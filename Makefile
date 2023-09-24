.DELETE_ON_ERROR:
build:
	cmake -Bbuild -G "MinGW Makefiles"
	cd build && make all

clean:
	powershell Remove-Item -Path build/* -Recurse
