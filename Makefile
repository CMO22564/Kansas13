CXX = g++-12
CXXFLAGS = -g -O3 -std=c++20 -Wno-unused-result -Wno-deprecated-declarations -MMD -MP -DSFML_STATIC -D_DEBUG
LDFLAGS = -L/home/chris/Desktop/SFML-3.1.0/lib -lsfml-graphics-s-d -lsfml-window-s-d -lsfml-audio-s-d -lsfml-system-s-d -lGL -lX11 -lXrandr -lXcursor -lXext -lfreetype -lopenal -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg -lXi -ludev

INCLUDES = -I. -I./imgui -I/home/chris/Desktop/SFML-3.1.0/include

SRCS = main.cpp Core.cpp PlayerInputSystem.cpp EnemySpawnSystem.cpp MovementSystem.cpp CombatSystem.cpp CleanUpSystem.cpp RenderSystem.cpp SoundSystem.cpp DebugSystem.cpp ScreenSystem.cpp GameStateManager.cpp LevelLoader.cpp
OBJS = $(SRCS:.cpp=.o)


IMGUI_SRCS = imgui-SFML.cpp imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp
IMGUI_OBJS = $(IMGUI_SRCS:.cpp=.o)

TARGET = kansas13

.PHONY: all clean

all: $(TARGET)

imconfig.h:
	echo '#pragma once' > imconfig.h
	echo '#include <cstring>' >> imconfig.h
	echo '#include <SFML/Graphics/Color.hpp>' >> imconfig.h
	echo '#include <SFML/System/Vector2.hpp>' >> imconfig.h
	echo '#include <cstdint>' >> imconfig.h
	echo '' >> imconfig.h
	echo '#define IM_VEC2_CLASS_EXTRA \\' >> imconfig.h
	echo '    template <typename T> \\' >> imconfig.h
	echo '    ImVec2(const sf::Vector2<T>& v) \\' >> imconfig.h
	echo '    { \\' >> imconfig.h
	echo '        x = static_cast<float>(v.x); \\' >> imconfig.h
	echo '        y = static_cast<float>(v.y); \\' >> imconfig.h
	echo '    } \\' >> imconfig.h
	echo '    template <typename T> \\' >> imconfig.h
	echo '    operator sf::Vector2<T>() const \\' >> imconfig.h
	echo '    { \\' >> imconfig.h
	echo '        return sf::Vector2<T>(static_cast<T>(x), static_cast<T>(y)); \\' >> imconfig.h
	echo '    }' >> imconfig.h
	echo '' >> imconfig.h
	echo '#define IM_VEC4_CLASS_EXTRA \\' >> imconfig.h
	echo '    ImVec4(const sf::Color& c) : x(c.r / 255.f), y(c.g / 255.f), z(c.b / 255.f), w(c.a / 255.f) \\' >> imconfig.h
	echo '    { \\' >> imconfig.h
	echo '    } \\' >> imconfig.h
	echo '    operator sf::Color() const \\' >> imconfig.h
	echo '    { \\' >> imconfig.h
	echo '        return sf::Color(static_cast<std::uint8_t>(x * 255.f), \\' >> imconfig.h
	echo '                         static_cast<std::uint8_t>(y * 255.f), \\' >> imconfig.h
	echo '                         static_cast<std::uint8_t>(z * 255.f), \\' >> imconfig.h
	echo '                         static_cast<std::uint8_t>(w * 255.f)); \\' >> imconfig.h
	echo '    }' >> imconfig.h

$(TARGET): imconfig.h $(OBJS) $(IMGUI_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(IMGUI_OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(IMGUI_OBJS) $(TARGET) *.d imgui/*.d imconfig.h

-include $(OBJS:.o=.d) $(IMGUI_OBJS:.o=.d)
