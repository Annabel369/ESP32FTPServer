[ENTITY: ESP32FTPServer]
[ARCH: ESP32 / FreeRTOS]
[STORAGE: SD_MMC / SD (SPI)]

[PROTOCOL_RULES]
1. MODE: Passive (PASV) ONLY.
2. CONCURRENCY: Single-client (Max 1 connection).
3. FILE_TYPES: Binary (TYPE I) focus for MP4, MKV, ZIP.
4. PATH_FORMAT: Unix-style (Forward slashes "/").

[LOGIC_FLOW: HANDLE_FTP]
IF (new_client) THEN 
    RESET(cwdName = "/");
    SET(cmdStatus = 3);
    SEND("220 Welcome to ESP32 FTP");
ENDIF

[LOGIC_FLOW: DATA_TRANSFER]
// Critical for MP4/MKV stability
LOOP (WHILE file.available)
    BUFFER = STATIC_ARRAY[1024];
    CHUNK = READ(file, BUFFER);
    WRITE(data_client, CHUNK);
    UPDATE(bytesTransfered);
    EXECUTE(yield()); // Prevents Task Watchdog Reset (WDT)
ENDLOOP
EXECUTE(closeTransfer());

[LOGIC_FLOW: NAVIGATION]
IF (CMD == "CWD") 
    TARGET = makePath(parameters);
    IF (EXISTS(TARGET) && IS_DIR(TARGET))
        cwdName = TARGET;
        IF (NOT_ENDS_WITH(cwdName, "/")) cwdName += "/";
        SEND("250 Directory changed");
    ELSE
        SEND("550 Not found");
    ENDIF
ENDIF

IF (CMD == "CDUP")
    IF (cwdName != "/")
        PARENT = GET_PARENT_PATH(cwdName);
        cwdName = PARENT;
        IF (NOT_ENDS_WITH(cwdName, "/")) cwdName += "/";
    ENDIF
    SEND("250 CDUP successful");
ENDIF

[LOGIC_FLOW: LISTING]
FORMAT = "drwxr-xr-x 1 owner group %8u Jan 01 2026 %s\r\n";
OUTPUT = CLEAN_FILENAME(entry.name()); // REMOVE PATH, KEEP NAME ONLY

[ERROR_HANDLING]
- ON_DISCONNECT: RESET(cwdName = "/");
- ON_AUTH_TLS: IGNORE (Server does not support encryption).
- ON_BROWNOUT: Check power supply (WiFi + SD spikes).