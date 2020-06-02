.PHONY: all clean permission

all: ${CURDIR}/nvidia-xrun-util

$(CURDIR)/nvidia-xrun-util: $(CURDIR)/nvidia-xrun-util.c
	gcc -g -O0 ${CURDIR}/nvidia-xrun-util.c -lm -o ${CURDIR}/nvidia-xrun-util

permission: $(CURDIR)/nvidia-xrun-util
	sudo chown root:root ${CURDIR}/nvidia-xrun-util && sudo chmod gu+s ${CURDIR}/nvidia-xrun-util

clean:
	-rm ${CURDIR}/nvidia-xrun-util
