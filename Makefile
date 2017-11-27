build:
	pio run

clean:
	pio run -t clean

upload:
	pio run -t upload

monitor:
	pio device monitor -b 115200

upmon: upload monitor

libjson:	
	@pio lib list --json-output | python -m json.tool