#ifndef CAMERA_OPENGL
#define CAMERA_OPENGL

	#ifndef GLM_OPENGL
	#define GLM_OPENGL
	#include "../../3rd/glm/glm/glm.hpp"
	#endif // !GLM_OPENGL

	#ifndef SFML_GLM
	#define SFML_GLM
	#include <SFML/Window.hpp> // sfml
	#endif // !SFML_GLM

#include "../ml.hpp"



#include "../../3rd/glm/glm/gtc/matrix_transform.hpp"
#include "../../3rd/glm/glm/gtx/transform.hpp"

class Camera {
public:
	glm::vec3 position;   // Позиция камеры.
	glm::vec3 view;   // Направление камеры.
	glm::vec3 up;   // Вертикальный вектор.
	glm::vec3 right;   // Вектор вправо.

	Camera() {
		position = { 0.0f, 0.0f, 3.0f };
		view = { 0.0f, 0.0f, -1.0f };
		up = { 0.0f, 1.0f, 0.0f };
		right = { 1.0f, 0.0f, 0.0f };
	}

	ml::mat4 get_model() {
		return ml::mat4(1.0f);
	}

	ml::mat4 get_view() {
		auto p = ml::vec3(position.x, position.y, position.z);
		auto v = ml::vec3(view.x, view.y, view.z);
		auto u = ml::vec3(up.x, up.y, up.z);
		return ml::lookAt(
			p,
			p + v,
			u
		);
	}

	glm::mat4 get_projection() {
		return glm::perspective(
			glm::radians(FoV),
			4.0f / 3.0f,
			0.1f,
			100.0f
		);
	}

	void keyboard() {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			position += view * speed;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			position -= view * speed;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			right = glm::normalize(cross(view, up));
			position -= right * speed;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			right = glm::normalize(cross(view, up));
			position += right * speed;
		}
	}

	void move_mouse(sf::Window& window) {
		auto buf = sf::Mouse::getPosition(window);
		int offX = posX - buf.x;
		posX = buf.x;
		int offY = posY - buf.y;
		posY = buf.y;
		// Вычисляем углы
		horizontalAngle += 0.005f * mouse * offX;
		verticalAngle += 0.005f * mouse * offY;
		//
		view = glm::vec3(cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);
		//
		right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f)
		);
		//
		up = cross(right, view);
	}

	void scroll(int dx) {
		if (dx > 0) {
			mouse += 0.1f;
		}
		if (dx < 0 && mouse > 0.1f) {
			mouse -= 0.1f;
		}
	}
	
	void set_pos(float x, float y, float z) {
		position = glm::vec3(x, y, z);
	}
	
	const glm::vec3 get_pos() {
		return position;
	}
	
	const glm::vec3 get_front() {
		return view;
	}
	
private:
	float FoV = 50.0f;
	float speed = 0.05f;
	float mouse = 1.0f;
	int posX = 400;
	int posY = 300;
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
};
#endif // !CAMERA_OPENGL