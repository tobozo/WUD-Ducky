MKSPIFFS    ?= mkspiffs
ESPTOOL     ?= esptool.py
DEVICE		:= /dev/cu.usbmodem01

default:
######  WTF arduino-cli providings --zip-path, --git-url, but not --zip-url
######  So there's no way to change branch on a downloaded library...
#	arduino-cli lib install --git-url https://github.com/chegewara/EspTinyUSB.git#1.3.4
	if [ ! -d "libraries/EspTinyUSB"]; then git clone --depth 1 --branch 1.3.4 https://github.com/chegewara/EspTinyUSB libraries/EspTinyUSB; fi
	arduino-cli compile .

.PHONY: buildfs
buildfs:
	mkdir -p build
	$(MKSPIFFS) -c data -p 256 -b 4096 -s 1507328 build/spiffs.bin

.PHONY: uploadfs
uploadfs:
	$(ESPTOOL) --chip esp32s2 --port ${DEVICE} --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio 2686976 build/spiffs.bin

.PHONY: upload
upload:
	arduino-cli upload -p ${DEVICE}
