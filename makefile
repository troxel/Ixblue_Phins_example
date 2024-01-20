#///////////////////
# Compiler and flags
CC=gcc
CFLAGS=-g -Wall
LIBS=-lm

# Directories
OBJDIR=./obj
BINDIR=./bin

# Create directories if they don't exist
$(shell mkdir -p $(OBJDIR))
$(shell mkdir -p $(BINDIR))

# Common source files
COMMON_SRC=udp_util.c phins_util.c
COMMON_OBJ=$(patsubst %.c,$(OBJDIR)/%.o,$(COMMON_SRC))

# Target 1 specific files
TARGET1=read_stdbin_phins.exe
TARGET1_SRC=read_stdbin_phins.c haversine.c 
TARGET1_OBJ=$(patsubst %.c,$(OBJDIR)/%.o,$(TARGET1_SRC))

# Target 2 specific files
TARGET2=write_phins.exe
TARGET2_SRC=write_phins.c
TARGET2_OBJ=$(patsubst %.c,$(OBJDIR)/%.o,$(TARGET2_SRC))

# Target 1 specific files 
TARGET3=write_phins_cmds.exe
TARGET3_SRC=write_phins_cmds.c 
TARGET3_OBJ=$(patsubst %.c,$(OBJDIR)/%.o,$(TARGET3_SRC))

# Targets
all: $(BINDIR)/$(TARGET1) $(BINDIR)/$(TARGET2) $(BINDIR)/$(TARGET3)

$(BINDIR)/$(TARGET1): $(COMMON_OBJ) $(TARGET1_OBJ)
	$(CC) $(COMMON_OBJ) $(TARGET1_OBJ) -Wall $(LIBS) -o $@ 

$(BINDIR)/$(TARGET2): $(COMMON_OBJ) $(TARGET2_OBJ)
	$(CC) $(COMMON_OBJ) $(TARGET2_OBJ) -Wall ${LIBS} -o $@

$(BINDIR)/$(TARGET3): $(COMMON_OBJ) $(TARGET3_OBJ)
	$(CC) $(COMMON_OBJ) $(TARGET3_OBJ) -Wall ${LIBS} -o $@

$(OBJDIR)/%.o: %.c
	$(CC)  -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(TARGET1) $(BINDIR)/$(TARGET2) $(BINDIR)/$(TARGET3)

.PHONY: all clean