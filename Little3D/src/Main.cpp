#include <iostream>
//#include <vld.h>
#include "App.h"
using namespace std;
int main(void) {
	std::cout << "Initiating..." << std::endl;
	L3DApp::App& app = L3DApp::App::GetInstance();
	int init_result = app.Init();

	std::cout << init_result << std::endl;
	if (0 == init_result){
		app.MainLoop();
	}
	app.Exit();
	return 0;
}