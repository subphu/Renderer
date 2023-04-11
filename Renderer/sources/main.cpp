#include <iostream>
#include <app.h>

int main() {
	App* app = new App();

	try {
		app->start();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}
