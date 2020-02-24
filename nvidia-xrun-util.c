#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

bool dry_run = false;

void trim_pair(char *str, int (*begin_match)(int c), int (*end_match)(int c)) {
  char *begin = str;
  char *end;

  if (str == NULL) {
    return;
  }

  while (begin_match(*begin)) {
    begin++;
  }

  if (*begin == '\0') {
    str[0] = '\0';
    return;
  }

  end = begin + strlen(begin) - 1;
  while (end > begin && end_match(*end)) {
    end--;
  }

  end = end + 1;
  *end = '\0';

  memmove(str, begin, end - begin + 1);
}

int is_quote(int c) { return c == '"'; }

int is_array_begin(int c) { return c == '('; }

int is_array_end(int c) { return c == ')'; }

void trim_space(char *str) { trim_pair(str, &isspace, &isspace); }
void trim_quote(char *str) { trim_pair(str, &is_quote, &is_quote); }
void trim_array(char *str) { trim_pair(str, &is_array_begin, &is_array_end); }

void parse_values(const char *data, char *values[], int count) {
  int index = 0;
  for (const char *end, *begin = data; *begin; begin = end) {
    // Skip leading spaces.
    while (isspace(*begin)) {
      ++begin;
    }

    if (is_quote(*begin)) {
      ++begin;
      end = begin;
      while (*end) {
        if (is_quote(*end)) {
          break;
        }
        ++end;
      }
      if (index >= count) {
        fprintf(stderr, "warn: values(%s) exceeded max count(%d)\n", data,
                count);
        return;
      }
      values[index] = strndup(begin, end - begin);
      ++index;
      if (*end) {
        ++end;
      }
    } else {
      end = begin;
      while (*end) {
        if (isspace(*end)) {
          break;
        }
        ++end;
      }
      if (index >= count) {
        fprintf(stderr, "warn: values(%s) exceeded max count(%d)\n", data,
                count);
        return;
      }
      values[index] = strndup(begin, end - begin);
      ++index;
    }
  }
}

void free_values(char *values[], int count) {
  for (int i = 0; i < count; ++i) {
    if (values[i]) {
      free(values[i]);
      values[i] = NULL;
    }
  }
}

typedef struct {
  bool enable_pm;
  bool remove_device;
  char device_bus_id[64];
  char controller_bus_id[64];
  int bus_rescan_wait_sec;
  char *modules_load[64];
  char *modules_unload[64];
} Conf;

bool conf_load(Conf *conf, const char *file) {
  FILE *f = fopen(file, "r");
  if (f == NULL) {
    fprintf(stderr, "open config file %s failed: %d\n", file, errno);
    return false;
  }
  char line[1024] = {'\0'};
  while (fgets(line, sizeof(line), f)) {
    trim_space(line);
    if (line[0] == '#') {
      continue; // Skip comment line
    }
    char *value = strchr(line, '=');
    if (value == NULL) {
      continue;
    }
    *value = '\0';
    ++value;
    trim_space(line);
    trim_space(value);
    if (strcmp(line, "ENABLE_PM") == 0) {
      trim_quote(value);
      conf->enable_pm = (value[0] == '1');
    } else if (strcmp(line, "REMOVE_DEVICE") == 0) {
      trim_quote(value);
      conf->remove_device = (value[0] == '1');
    } else if (strcmp(line, "CONTROLLER_BUS_ID") == 0) {
      trim_quote(value);
      snprintf(conf->controller_bus_id, sizeof(conf->controller_bus_id), "%s",
               value);
    } else if (strcmp(line, "DEVICE_BUS_ID") == 0) {
      trim_quote(value);
      snprintf(conf->device_bus_id, sizeof(conf->device_bus_id), "%s", value);
    } else if (strcmp(line, "BUS_RESCAN_WAIT_SEC") == 0) {
      trim_quote(value);
      conf->bus_rescan_wait_sec = atoi(value);
    } else if (strcmp(line, "MODULES_LOAD") == 0) {
      trim_array(value);
      parse_values(value, conf->modules_load,
                   sizeof(conf->modules_load) / sizeof(conf->modules_load[0]));
    } else if (strcmp(line, "MODULES_UNLOAD") == 0) {
      trim_array(value);
      parse_values(value, conf->modules_unload,
                   sizeof(conf->modules_unload) /
                       sizeof(conf->modules_unload[0]));
    } else {
      fprintf(stderr, "ignore: unknown config item '%s'\n", line);
    }
  }
  fclose(f);
  return true;
}

void conf_free(Conf *conf) {
  free_values(conf->modules_load, sizeof(conf->modules_load)/sizeof(conf->modules_load[0]));
  free_values(conf->modules_unload, sizeof(conf->modules_unload)/sizeof(conf->modules_unload[0]));
}

void conf_dump(const Conf *conf) {
  printf("enable_pm\n\t%d\n"
         "remove_device\n\t%d\n"
         "device_bus_id\n\t%s\n"
         "controller_bus_id\n\t%s\n"
         "bus_rescan_wait_sec\n\t%d\n",
         conf->enable_pm, conf->remove_device, conf->device_bus_id,
         conf->controller_bus_id, conf->bus_rescan_wait_sec);
  printf("modules_load\n");
  int i;
  for (i = 0; i < sizeof(conf->modules_load) / sizeof(conf->modules_load[0]);
       ++i) {
    if (conf->modules_load[i]) {
      printf("\t%s\n", conf->modules_load[i]);
    }
  }
  printf("modules_unload\n");
  for (i = 0;
       i < sizeof(conf->modules_unload) / sizeof(conf->modules_unload[0]);
       ++i) {
    if (conf->modules_unload[i]) {
      printf("\t%s\n", conf->modules_unload[i]);
    }
  }
}

void update_file(const char *file, const char *str) {
  if (dry_run) {
    printf(">>Dry run. Command: echo '%s' > %s\n", str, file);
    return;
  }
  int fd = open(file, O_WRONLY);
  if (fd != -1) {
    write(fd, str, strlen(str));
    close(fd);
  }
}

void turn_off_gpu(const Conf *conf) {
  char file[PATH_MAX] = {'\0'};
  if (conf->remove_device) {
    puts("Removing Nvidia bus from the kernel");
    snprintf(file, sizeof(file), "/sys/bus/pci/devices/%s/remove",
             conf->device_bus_id);
    update_file(file, "1");
  } else {
    puts("Enabling powersave for the graphic card");
    snprintf(file, sizeof(file), "/sys/bus/pci/devices/%s/power/control",
             conf->device_bus_id);
    update_file(file, "auto");
  }

  puts("Enabling powersave for the PCIe controller");
  snprintf(file, sizeof(file), "/sys/bus/pci/devices/%s/power/control",
           conf->controller_bus_id);
  update_file(file, "auto");
}

void turn_on_gpu(const Conf *conf) {
  puts("Turning the PCIe controller on to allow card rescan");
  char file[PATH_MAX] = {'\0'};
  snprintf(file, sizeof(file), "/sys/bus/pci/devices/%s/power/control",
           conf->controller_bus_id);
  update_file(file, "on");

  puts("Waiting 1 second");
  if (dry_run) {
    printf(">>Dry run. Command: sleep 1\n");
  } else {
    sleep(1);
  }

  snprintf(file, sizeof(file), "/sys/bus/pci/devices/%s", conf->device_bus_id);
  struct stat st;
  if (stat(file, &st) != 0 || !S_ISDIR(st.st_mode)) {
    puts("Rescanning PCI devices");
    update_file("/sys/bus/pci/rescan", "1");
    printf("Waiting %d second for rescan\n", conf->bus_rescan_wait_sec);
    if (dry_run) {
      printf(">>Dry run. Command: sleep %d\n", conf->bus_rescan_wait_sec);
    } else {
      sleep(conf->bus_rescan_wait_sec);
    }
  }

  puts("Turning the card on");
  snprintf(file, sizeof(file), "/sys/bus/pci/devices/%s/power/control",
           conf->device_bus_id);
  update_file(file, "on");
}

void load_module(const char *module) {
  printf("Loading module %s\n", module);

  if (dry_run) {
    printf(">>Dry run. Command: modprobe '%s'\n", module);
    return;
  }

  pid_t pid = fork();

  if (pid == -1) {
    perror("fork");
    return;
  }

  if (pid > 0) {
    waitpid(pid, NULL, 0);
    return;
  }

  char* args[64] = {NULL};
  args[0] = strdup("/usr/bin/timeout");
  args[1] = strdup("10");
  args[2] = strdup("/usr/bin/modprobe");
  parse_values(module, args + 3, sizeof(args)/sizeof(args[0]) - 4);
  execvp(args[0], args);
  free_values(args, sizeof(args)/sizeof(args[0]));
  exit(EXIT_FAILURE);
}

void unload_module(const char *module) {
  printf("Unloading module %s\n", module);
  if (dry_run) {
    printf(">>Dry run. Command: modprobe -r '%s'\n", module);
    return;
  }

  pid_t pid = fork();

  if (pid == -1) {
    perror("fork");
    return;
  }

  if (pid > 0) {
    waitpid(pid, NULL, 0);
    return;
  }

  execl("/usr/bin/timeout", "/usr/bin/timeout", "10", "/usr/bin/modprobe", "-r", module, NULL);
  exit(EXIT_FAILURE);
}

void load_modules(const Conf *conf) {
  int i;

  for (i = 0; i < sizeof(conf->modules_load) / sizeof(conf->modules_load[0]);
       ++i) {
    if (conf->modules_load[i]) {
      load_module(conf->modules_load[i]);
    }
  }
}

void unload_modules(const Conf *conf) {
  int i;

  for (i = 0;
       i < sizeof(conf->modules_unload) / sizeof(conf->modules_unload[0]);
       ++i) {
    if (conf->modules_unload[i]) {
      unload_module(conf->modules_unload[i]);
    }
  }
}

void usage(int argc, char *argv[]) {
  fprintf(stderr,
          "Usage: %s "
          "<turn_off_gpu|turn_on_gpu|force_turn_off_gpu|force_turn_on_gpu|load_"
          "modules|unload_modules|dump_conf> [dry_run]\n",
          argv[0]);
}

int main(int argc, char *argv[]) {
  Conf conf = {'\0'};
  if (!conf_load(&conf, "/etc/default/nvidia-xrun")) {
    return EXIT_FAILURE;
  }

  if (argc > 1 && argc < 4) {
    dry_run = (argc > 2 && strcmp(argv[2], "1") == 0);
    if (!dry_run) {
      if (setuid(0) == -1) {
        perror("setuid");
        return EXIT_FAILURE;
      }
      if (setgid(0) == -1) {
        perror("setgid");
        return EXIT_FAILURE;
      }
    }

    if (strcmp(argv[1], "turn_off_gpu") == 0) {
      if (conf.enable_pm) {
        turn_off_gpu(&conf);
      }
      return 0;
    } else if (strcmp(argv[1], "turn_on_gpu") == 0) {
      if (conf.enable_pm) {
        turn_on_gpu(&conf);
      }
      return 0;
    } else if (strcmp(argv[1], "force_turn_off_gpu") == 0) {
      turn_off_gpu(&conf);
      return 0;
    } else if (strcmp(argv[1], "force_turn_on_gpu") == 0) {
      turn_on_gpu(&conf);
      return 0;
    } else if (strcmp(argv[1], "load_modules") == 0) {
      load_modules(&conf);
      return 0;
    } else if (strcmp(argv[1], "unload_modules") == 0) {
      unload_modules(&conf);
      return 0;
    } else if (strcmp(argv[1], "dump_conf") == 0) {
      conf_dump(&conf);
      return 0;
    }
  }

  usage(argc, argv);

  return EXIT_FAILURE;
}
