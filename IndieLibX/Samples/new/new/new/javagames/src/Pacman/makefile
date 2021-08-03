# Generic Makefile (c) 2013 Jörg Seebohn
# Version 1.0
# Use it at your own risk !
# This Makefile depends on GNU make.


#########################
# Project Specific Part #
#########################
Project := game
Modes   := Debug #Release# - uncomment for release build too`

# Directory where target binary is created and object files
TargetDir         := bin
# Search Path For Include Files (InclPaths is included before mode specific InclPaths_MODE)
InclPaths         := .
InclPaths_Debug   :=
InclPaths_Release :=
# Defined values used by C preprocessor (Defines is included before mode specific Defines_MODE)
Defines           := 
Defines_Debug     := DEBUG
Defines_Release   := RELEASE
# List of Library Names (Libs is included before mode specific Libs_MODE)
Libs              := SDL SDL_image SDL_ttf SDL_mixer SDL_gfx
Libs_Debug        :=
Libs_Release      :=
# Search Paths for Libraries (LibPaths is included before mode specific LibPaths_MODE)
LibPaths          :=
LibPaths_Debug    :=
LibPaths_Release  :=
# Compiler specific flags (CFlags is included before mode specific CFlags_MODE)
CFlags            := -std=c99 -pedantic -Wall -Wextra -Wshadow -Wcast-qual -Wwrite-strings -Wstrict-prototypes -Wformat-nonliteral -Wformat-y2k
CFlags_Debug      := -g
CFlags_Release    := -O2
# Linker specific flags (LFlags is included before mode specific LFlags_MODE)
LFlags            :=
LFlags_Debug      := -g
LFlags_Release    := -O2

# Add Your Source file here
# Use $(wildcard dir/*.c) if you want to include files from directory 'dir'
Src := $(wildcard src/*.c)

##########################
# Compiler Specific Part #
##########################
IncPathFlag := -I
DefineFlag  := -D
LibraryFlag := -l
LibPathFlag := -L
# -MMD flag let gcc generate a dependency file "objectfilename.d" in addition to the object file "objectfilename.o"
CC          := gcc -MMD
LD          := gcc

#########################
# Generic Template Part #
#########################
all: $(Modes)

clean:
	@echo clean: $(Project)
	@echo rm -rf $(TargetDir)

test:	Release
	$(call TargetFilename,Release)

$(Modes:%=clean_%):
	@echo rm -rf $(TargetDir)/$(subst clean_,,$@)/$(Project)/

$(Modes:%=init_%):
	@mkdir -p $(TargetDir)/$(subst init_,,$@)/$(Project)

define TargetFilename
$(TargetDir)/$(Project)$(subst _Release,,_$(1))
endef

define ObjectFilename
$(TargetDir)/$(1)/$(Project)/$(subst /,!,$(2)).o
endef

define CompileObject
$(call ObjectFilename,$(1),$(2)): $(2)
	@echo cc: '$$<'
	@$$(CC_$(1)) -c -o '$$@' '$$<'
endef

define TargetTemplate
CC_$(1) := $(CC) $(Defines:%=$(DefineFlag)%) $$(Defines_$(1):%=$(DefineFlag)%) $(InclPaths:%=$(IncPathFlag)%) $$(InclPaths_$(1):%=$(IncPathFlag)%) $(CFlags) $$(CFlags_$(1))

Objects_$(1)  := $(foreach file,$(Src),$(call ObjectFilename,$(1),$(file)))

$(1):	init_$(1) $(call TargetFilename,$(1))

$(call TargetFilename,$(1)): $$(Objects_$(1))
	@echo ld: '$$@'
	@$(LD) $(LFlags) $$(LFlags_$(1)) -o '$$@' $$(^:%='%') $(LibPaths:%=$(LibPathFlag)%) $$(LibPaths_$(1):%=$(LibPathFlag)%) $(Libs:%=$(LibraryFlag)%) $$(Libs_$(1):%=$(LibraryFlag)%)

$(foreach file,$(Src),$(eval $(call CompileObject,$(1),$(file))))

# Include compiler generated dependency files
-include $$(Objects_$(1):.o=.d)

endef

$(foreach mode,$(Modes),$(eval $(call TargetTemplate,$(mode))))