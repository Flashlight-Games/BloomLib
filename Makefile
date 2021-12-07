ifeq ($(OS),Windows_NT)
    targetLib = lglfw3dll
	targetType = exe
	runCmd = build/$(appname).$(targetType)
else
    targetLib = lglfw
	targetType = out
	runCmd = ./build/$(appname).$(targetType)
endif

# Name of the app
appname = app


main: src/main.cpp $(wildcard src/**/*.cpp)
	g++ -std=c++11 -I include/ -L lib/ $^ src/glad.cpp  -$(targetLib) -o build/$(appname).$(targetType)

run:
	make main
	$(runCmd)

#--------------
*.o: *.cpp *.h
	g++ -std=c++11 -c $^ 

# ----CLEAN----
CLEAN = rm *.o
# -------------