TARGET 	:= maple

all: src/main.cpp
	platformio run -e $(TARGET)

install: .pioenvs/$(TARGET)/firmware.bin
	platformio run --target upload -e $(TARGET)

clean:
	platformio run --target clean

com:
	platformio device monitor --echo

.PHONY: clean
