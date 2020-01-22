.PHONY: deps all vpk

TITLE = Gordon Vita 002
TITLE_ID = GVITAV002
TARGET = GVITAV002
VERSION = 00.30

CXX = arm-vita-eabi-g++
STRIP = arm-vita-eabi-strip

SRC = $(foreach dir,src, $(wildcard $(dir)/*.cpp))

CXXWARNINGS = -Wall -Wextra -Wformat=2 -Winit-self -Wmissing-include-dirs \
	-Wunknown-pragmas -Wduplicated-cond
CXXOPTIMATION = -O3
CXXFLAGS = -std=gnu++17 $(CXXWARNINGS) $(CXXOPTIMATION) -Ideps/libvita2d/include
LIBS = deps/libvita2d/libvita2d.a \
	-lpthread -lSceDisplay_stub -lSceGxm_stub -lSceVshBridge_stub  \
	-lSceSysmodule_stub -lSceCtrl_stub -lSceAppMgr_stub -lScePaf_stub \
	-lSceRtc_stub -lSceNet_stub -lSceNetCtl_stub -lSceAppUtil_stub \
	-lScePower_stub -lScePgf_stub -lfreetype -lz -lpng

OUT := out
OBJ := $(addprefix $(OUT)/, $(SRC:src/%.cpp=%.o))

all: deps vpk
vpk: $(OUT)/$(TARGET).vpk

$(OUT)/$(TARGET).vpk: $(OUT)/eboot.bin $(OUT)/param.sfo
	vita-pack-vpk -s $(OUT)/param.sfo -b $(OUT)/eboot.bin \
		--add sce_sys/icon0.png=sce_sys/icon0.png \
		--add sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png \
		--add sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png \
		--add sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml \
	$@

$(OUT)/eboot.bin: $(OUT)/$(TARGET).velf
	vita-make-fself -c $< $@

$(OUT)/param.sfo:
	vita-mksfoex -d PARENTAL_LEVEL=1 -s APP_VER=$(VERSION) -s VERSION=$(VERSION) \
	-s TITLE_ID=$(TITLE_ID) "$(TITLE)" $@

$(OUT)/$(TARGET).velf: $(OUT)/$(TARGET).elf
	$(STRIP) --strip-unneeded $<
	vita-elf-create $< $@

$(OUT)/$(TARGET).elf: $(OBJ)
	$(CXX) $(CXXFLAGS) -Wl,-q -o $@ $^ $(LIBS)

$(OUT):
	@mkdir -p $@

$(OUT)/%.o : src/%.cpp | $(OUT)
	$(CXX) $(CXXFLAGS) -Wl,-q -c -o $@ $<

clean:
	@rm -rf $(OUT)/

clean-all: clean clean-deps

vpksend1: $(OUT)/$(TARGET).vpk
	curl -T $(OUT)/$(TARGET).vpk ftp://192.168.2.15:1337/ux0:/

vpksend2: $(OUT)/$(TARGET).vpk
	curl -T $(OUT)/$(TARGET).vpk ftp://192.168.2.14:1337/ux0:/

send1: $(OUT)/eboot.bin
	@curl -s -T $(OUT)/eboot.bin ftp://192.168.2.15:1337/ux0:/app/$(TITLE_ID)/

send2: $(OUT)/eboot.bin
	curl -T $(OUT)/eboot.bin ftp://192.168.2.14:1337/ux0:/app/$(TITLE_ID)/

deps:
	@make -C deps/libvita2d

clean-deps:
	@make -C deps/libvita2d clean
