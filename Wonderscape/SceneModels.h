#include "Config.h"
#include "Model.h"
#include "Texture.h"
#include "Paredes.h"
#include "ShaderClassF.h"
#include "VAOF.h"
#include "VBOF.h"
#include "EBOF.h"
#include "TextureF.h"
#include "ArraysFaces.h"
#include "AudioManager.h"

float r = 2.465f;
bool inLimitedZone = false;

Camera cameras(width, height, glm::vec3(0.0f, 10.0f, 1.0f));
glm::vec3 minLimite = glm::vec3(0.0f);
glm::vec3 maxLimite = glm::vec3(0.0f);
glm::vec3 centerCubo = glm::vec3(0.0f);

const char* Models[] = {
	"models/maravillas/textures/base_baseColor.png",//ColiseoRomano
	"models/maravillas/textures/Material.002_baseColor.png",//TajMajal
	"models/maravillas/textures/Material.003_baseColor.png",//MachuPichu
	"models/maravillas/textures/Material.005_baseColor.png",//CristoRedentor
	"models/maravillas/textures/Material.007_baseColor.png",//Petra
	"models/maravillas/textures/Material.010_baseColor.png",//MurallaChina
	"models/maravillas/textures/Material.016_baseColor.png"//ChichenItza
};

const char* Faces[] = {
	"models/Skyboxes/FACES/MURALLACHINA.png",
	"models/Skyboxes/FACES/CIELOMURALLA.png",
	"models/Skyboxes/FACES/PETRA.png",
	"models/Skyboxes/FACES/CIELOPETRA.png",
	"models/Skyboxes/FACES/CRISTO.png",
	"models/Skyboxes/FACES/CIELOCRISTO.png",
	"models/Skyboxes/FACES/MACHUPICHU.png",
	"models/Skyboxes/FACES/CIELOMACHU.png",
	"models/Skyboxes/FACES/CHICHEN.png",
	"models/Skyboxes/FACES/CIELOCHICHEN.png",
	"models/Skyboxes/FACES/COLISEO.png",
	"models/Skyboxes/FACES/CIELOCOLISEO.png",
	"models/Skyboxes/FACES/TALMAJAL.png",
	"models/Skyboxes/FACES/CIELOTALMAJAL.png"
};

bool flashlightOn = false, informationScreen = false;
float flashlightCutOff = cos(glm::radians(12.1f));
float flashlightOuterCutOff = cos(glm::radians(17.1f));
float flashlightIntensity = 2;
int radieon = 1;
int index = 0;
AudioManager* sound;

float Colors[] = {
	rgb(220), rgb(240), rgb(247),//light normal
	rgb(102), rgb(102), rgb(204),//Nocturn
	rgb(135), rgb(46), rgb(230),//ultra violet
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		std::cout << "cameras Position: ("
			<< cameras.Position.x << ", "
			<< cameras.Position.y << ", "
			<< cameras.Position.z << ")\n";

		std::cout << "cameras Orientation: ("
			<< cameras.Orientation.x << ", "
			<< cameras.Orientation.y << ", "
			<< cameras.Orientation.z << ")\n";
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		sound->playClickSound("media/clicklight.mp3");
		flashlightOn = !flashlightOn;
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS && flashlightOn) {
		if (flashlightIntensity < 5) {
			flashlightIntensity += 2;
		}
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && flashlightOn) {
		if (flashlightIntensity > 1) {
			flashlightIntensity -= 2;
		}
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS && flashlightOn) {
		if (radieon == 0) {
			flashlightCutOff = cos(glm::radians(12.1f));
			flashlightOuterCutOff = cos(glm::radians(17.1f));
		}
		else if (radieon == 1) {
			flashlightCutOff = cos(glm::radians(12.1f + 5.88f));
			flashlightOuterCutOff = cos(glm::radians(17.1f + 5.88f));
		}
		else if (radieon == 2) {
			flashlightCutOff = cos(glm::radians(12.1f + 9.88f));
			flashlightOuterCutOff = cos(glm::radians(17.1f + 9.88f));
		}

		if (radieon >= 0 && radieon <= 2) {
			radieon++;
			if (radieon == 3) {
				radieon = 0;
			}
		}
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS && flashlightOn) {
		if (index >= 0 && index <= 6) {
			index += 3;

			if (index == 9) {
				index = 0;
			}
		}
	}
}
/*std::vector<glm::vec3> getCubeFaceVertices(const glm::vec3& center, float radius, const std::string& face) {
	// V rtices base (sin centrar)
	glm::vec3 frontBottomLeft(-radius, -radius, radius);
	glm::vec3 frontBottomRight(radius, -radius, radius);
	glm::vec3 frontTopRight(radius, radius, radius);
	glm::vec3 frontTopLeft(-radius, radius, radius);

	glm::vec3 backBottomLeft(-radius, -radius, -radius);
	glm::vec3 backBottomRight(radius, -radius, -radius);
	glm::vec3 backTopRight(radius, radius, -radius);
	glm::vec3 backTopLeft(-radius, radius, -radius);

	// Seleccionamos los v rtices seg n la cara solicitada
	std::vector<glm::vec3> vertices;

	if (face == "front") {
		vertices = { frontBottomLeft, frontBottomRight, frontTopRight, frontTopLeft };
	}
	else if (face == "back") {
		vertices = { backBottomRight, backBottomLeft, backTopLeft, backTopRight };
	}
	else if (face == "left") {
		vertices = { backBottomLeft, frontBottomLeft, frontTopLeft, backTopLeft };
	}
	else if (face == "right") {
		vertices = { frontBottomRight, backBottomRight, backTopRight, frontTopRight };
	}
	else if (face == "top") {
		vertices = { frontTopLeft, frontTopRight, backTopRight, backTopLeft };
	}
	else if (face == "bottom") {
		vertices = { backBottomLeft, backBottomRight, frontBottomRight, frontBottomLeft };
	}
	else {
		// Cara no v lida, devolvemos una cara por defecto (front)
		vertices = { frontBottomLeft, frontBottomRight, frontTopRight, frontTopLeft };
	}

	// Aplicamos la posici n del centro
	for (auto& vertex : vertices) {
		vertex += center;
	}

	return vertices;
}*/
void ValuesOfTheLimitsOfWonder(int Tecla) {
	if (Tecla == 1) {
		cameras.Position = glm::vec3(-22.6626, 4.41481, 34.4906);
		cameras.Orientation = glm::normalize(glm::vec3(0.773963, 0.0138641, 0.633079));
		centerCubo = glm::vec3(-20.1976, 3.79907, 36.9556);//aprobado
		minLimite = centerCubo - glm::vec3(r, r, r);
		maxLimite = centerCubo + glm::vec3(r, r + 2.5, r);
		minLimite.y = centerCubo.y + 0.5;
		cameras.Object(centerCubo, 2.55f);
	}
	if (Tecla == 2) {
		cameras.Position = glm::vec3(-23.7076, 5.78611, -36.0866);
		cameras.Orientation = glm::normalize(glm::vec3(0.593846, -0.105432, -0.797643));
		centerCubo = glm::vec3(-21.2426, 3.64704, -38.5516);//aprobado
		minLimite = centerCubo - glm::vec3(r, r, r);
		maxLimite = centerCubo + glm::vec3(r, r + 2.5, r);
		minLimite.y = centerCubo.y + 0.5;
		cameras.Object(centerCubo, 3.1f);
	}
	if (Tecla == 3) {
		cameras.Position = glm::vec3(-38.8936, 7.2532, 2.02814);
		cameras.Orientation = glm::normalize(glm::vec3(0.707867, 0.319353, -0.630033));
		centerCubo = glm::vec3(-36.6286, 6.55324, -0.436863);
		minLimite = centerCubo - glm::vec3(r - 0.2, r, r - 0.2);  // Y no se reduce
		maxLimite = centerCubo + glm::vec3(r, r + 1.6, r);  // Y s  sube
		minLimite.y = centerCubo.y + 0.5;
		cameras.Object(centerCubo, 2.43f);
	}
	if (Tecla == 4) {
		cameras.Position = glm::vec3(18.9332, 4.41443, -32.6582);
		cameras.Orientation = glm::normalize(glm::vec3(0.67258, -0.0583779, -0.737719));
		centerCubo = glm::vec3(21.2034, 3.90242, -35.1232);
		minLimite = centerCubo - glm::vec3(r, r, r);  // Y no se reduce
		maxLimite = centerCubo + glm::vec3(r, r + 1.75, r);  // Y s  sube
		minLimite.y = centerCubo.y + 0.5;
		cameras.Object(centerCubo, 2.40f);
	}
	if (Tecla == 5) {
		cameras.Position = glm::vec3(34.1383, 2.94504, 2.51183);
		cameras.Orientation = glm::normalize(glm::vec3(0.62735, -0.0215879, -0.778436));
		centerCubo = glm::vec3(36.3341, 2.43712, 0.046831);
		minLimite = centerCubo - glm::vec3(r, r, r);  // Y no se reduce
		maxLimite = centerCubo + glm::vec3(r, r + 2.5, r);  // Y s  sube
		minLimite.y = centerCubo.y + 0.5;
		cameras.Object(centerCubo, 2.35f);
	}
	if (Tecla == 6) {
		cameras.Position = glm::vec3(18.6961, 2.33117, 38.102);
		cameras.Orientation = glm::normalize(glm::vec3(0.823214, -0.0226236, -0.56728));
		centerCubo = glm::vec3(21.1115, 1.83117, 35.8408);
		minLimite = centerCubo - glm::vec3(r, r, r);  // Y no se reduce
		maxLimite = centerCubo + glm::vec3(r, r + 2.2, r);  // Y s  sube
		minLimite.y = centerCubo.y + 0.5;
		cameras.Object(centerCubo, 2.32f);
	}
	if (Tecla == 7) {
		cameras.Position = glm::vec3(2.19636, 2.53347, -0.59396);
		cameras.Orientation = glm::normalize(glm::vec3(-0.84389, 0.0674655, 0.532258));
		centerCubo = glm::vec3(-0.0592385, 2.03347, 1.87104);
		minLimite = centerCubo - glm::vec3(r, r, r);  // Y no se reduce
		maxLimite = centerCubo + glm::vec3(r, r + 2.5, r);  // Y s  sube
		minLimite.y = centerCubo.y + 0.5;
		cameras.Object(centerCubo, 2.32f);
	}
}

bool SceneModels(int wonder, GLFWwindow* window, Model& model) {
	AudioManager audioCLICK;
	audioCLICK.setMusicVolume(0.3f);
	sound = &audioCLICK;


	// Cargar audio
	AudioManager audio;
	audio.playBackgroundMusic("media/exploration.mp3");
	audio.setMusicVolume(0.3f);
	audio.setEffectsVolume(0.4f);

	// Imagen informativa
	ShaderF backgroundShader("background.vert", "background.frag");

	float backgroundVertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int indexes[] = {
		0, 1, 2,
		2, 3, 0
	};

	VAOF vaoF;
	vaoF.Bind();
	VBOF vboF(backgroundVertices, sizeof(backgroundVertices));
	EBOF eboF(indexes, sizeof(indexes));

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	vaoF.Unbind(); vboF.Unbind(); eboF.Unbind();

	std::vector<std::string> routesWonders = {
	   "images/info/great wall of china.jpg",
	   "images/info/petra.jpg",
	   "images/info/christ the reddemer.jpg",
	   "images/info/machu picchu.jpg",
	   "images/info/chichen itza.jpg",
	   "images/info/roman colosseum.jpg",
	   "images/info/taj mahal.jpg"
	};

	TextureF backgroundInfo(routesWonders[wonder - 1].c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

	// Modelos
	glfwMakeContextCurrent(window);
	gladLoadGL();

	Shader shaderProgram_Scene("default_models.vert", "default_models.frag");

	glm::vec4 lightColor = glm::vec4(1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);

	lightModel = glm::translate(lightModel, lightPos);
	shaderProgram_Scene.Activate();

	glUniform4f(glGetUniformLocation(shaderProgram_Scene.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram_Scene.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3fv(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightPos"), 1, glm::value_ptr(cameras.Position));
	glUniform3fv(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightDir"), 1, glm::value_ptr(cameras.Orientation));
	glEnable(GL_DEPTH_TEST);

	glfwSetKeyCallback(window, keyCallback);

	bool information = false;
	static bool keyIWasPressed = false;

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			shaderProgram_Scene.Activate();
			glUniform1f(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightIntensity"), 0.0f);
			inLimitedZone = false;
			flashlightIntensity = 1.0f;
			flashlightOn = false;
			flashlightCutOff = cos(glm::radians(12.1f));
			flashlightOuterCutOff = cos(glm::radians(17.1f));
			radieon = 1;
			index = 0;
			return true;
		}

		// Logica para el panel de informacion
		bool keyIIsPressed = glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS;

		// Solo cambia el estado cuando la tecla pasa de no presionada a presionada
		if (keyIIsPressed && !keyIWasPressed) {
			information = !information; // Alternar estado
		}
		keyIWasPressed = keyIIsPressed;

		if (information) {
			// Fondo con textura
			glDisable(GL_DEPTH_TEST);
			backgroundShader.Activate();
			vaoF.Bind();
			backgroundInfo.Bind();
			backgroundInfo.texUnit(backgroundShader.ID, "backgroundTexture", 0);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glEnable(GL_DEPTH_TEST);
		}
		else {
			cameras.Inputs(window);
			model.Draw(shaderProgram_Scene, cameras);
			cameras.updateMatrix(45.0f, 0.1f, 20.0f);

			if (wonder == 1) {
				renderParedes(Right_Maravilla1, 44, window, cameras, Faces[0]);
				renderParedes(Left_Maravilla1, 44, window, cameras, Faces[0]);
				renderParedes(Front_Maravilla1, 44, window, cameras, Faces[0]);
				renderParedes(Back_Maravilla1, 44, window, cameras, Faces[0]);
				renderParedes(Top_Maravilla1, 44, window, cameras, Faces[1]);
			}
			if (wonder == 2) {
				renderParedes(Right_Maravilla2, 44, window, cameras, Faces[2]);
				renderParedes(Left_Maravilla2, 44, window, cameras, Faces[2]);
				renderParedes(Front_Maravilla2, 44, window, cameras, Faces[2]);
				renderParedes(Back_Maravilla2, 44, window, cameras, Faces[2]);
				renderParedes(Top_Maravilla2, 44, window, cameras, Faces[3]);
			}
			if (wonder == 3) {
				renderParedes(Right_Maravilla3, 44, window, cameras, Faces[4]);
				renderParedes(Left_Maravilla3, 44, window, cameras, Faces[4]);
				renderParedes(Front_Maravilla3, 44, window, cameras, Faces[4]);
				renderParedes(Back_Maravilla3, 44, window, cameras, Faces[4]);
				renderParedes(Top_Maravilla3, 44, window, cameras, Faces[5]);
			}
			if (wonder == 4) {
				renderParedes(Right_Maravilla4, 44, window, cameras, Faces[6]);
				renderParedes(Left_Maravilla4, 44, window, cameras, Faces[6]);
				renderParedes(Front_Maravilla4, 44, window, cameras, Faces[6]);
				renderParedes(Back_Maravilla4, 44, window, cameras, Faces[6]);
				renderParedes(Top_Maravilla4, 44, window, cameras, Faces[7]);
			}
			if (wonder == 5) {
				renderParedes(Right_Maravilla5, 44, window, cameras, Faces[8]);
				renderParedes(Left_Maravilla5, 44, window, cameras, Faces[8]);
				renderParedes(Front_Maravilla5, 44, window, cameras, Faces[8]);
				renderParedes(Back_Maravilla5, 44, window, cameras, Faces[8]);
				renderParedes(Top_Maravilla5, 44, window, cameras, Faces[9]);
			}
			if (wonder == 6) {
				renderParedes(Right_Maravilla6, 44, window, cameras, Faces[10]);
				renderParedes(Left_Maravilla6, 44, window, cameras, Faces[10]);
				renderParedes(Front_Maravilla6, 44, window, cameras, Faces[10]);
				renderParedes(Back_Maravilla6, 44, window, cameras, Faces[10]);
				renderParedes(Top_Maravilla6, 44, window, cameras, Faces[11]);
			}
			if (wonder == 7) {
				renderParedes(Right_Maravilla7, 44, window, cameras, Faces[12]);
				renderParedes(Left_Maravilla7, 44, window, cameras, Faces[12]);
				renderParedes(Front_Maravilla7, 44, window, cameras, Faces[12]);
				renderParedes(Back_Maravilla7, 44, window, cameras, Faces[12]);
				renderParedes(Top_Maravilla7, 44, window, cameras, Faces[13]);
			}
			//------------------------------------------------------------------------------
			glDepthFunc(GL_LEQUAL);
			glm::mat4 view = glm::mat4(0.1f);
			glm::mat4 projection = glm::mat4(0.1f);
			// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
			// The last row and column affect the translation of the skybox (which we don't want to affect)
			view = glm::mat4(glm::mat3(glm::lookAt(cameras.Position, cameras.Position + cameras.Orientation, cameras.Up)));
			projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
			//glDisable(GL_DEPTH_TEST);

			// Switch back to the normal depth function
			glDepthFunc(GL_LESS);
			//------------------------------------------------------------------------------

			if (inLimitedZone == false) {
				if (wonder == 1) {
					Texture Maravilla(Models[5], "albedo", 0);
					for (auto& mesh : model.meshes) {
						mesh.textures.clear();
						mesh.textures.push_back(Maravilla);
					}
					inLimitedZone = true;
					ValuesOfTheLimitsOfWonder(1);
				}
				if (wonder == 2) {
					Texture Maravilla(Models[4], "albedo", 0);
					for (auto& mesh : model.meshes) {
						mesh.textures.clear();
						mesh.textures.push_back(Maravilla);
					}
					inLimitedZone = true;
					ValuesOfTheLimitsOfWonder(2);
				}
				if (wonder == 3) {
					Texture Maravilla(Models[3], "albedo", 0);
					for (auto& mesh : model.meshes) {
						mesh.textures.clear();
						mesh.textures.push_back(Maravilla);
					}
					inLimitedZone = true;
					ValuesOfTheLimitsOfWonder(3);
				}
				if (wonder == 4) {
					Texture Maravilla(Models[2], "albedo", 0);
					for (auto& mesh : model.meshes) {
						mesh.textures.clear();
						mesh.textures.push_back(Maravilla);
					}
					inLimitedZone = true;
					ValuesOfTheLimitsOfWonder(4);
				}
				if (wonder == 5) {
					Texture Maravilla(Models[6], "albedo", 0);
					for (auto& mesh : model.meshes) {
						mesh.textures.clear();
						mesh.textures.push_back(Maravilla);
					}
					inLimitedZone = true;
					ValuesOfTheLimitsOfWonder(5);
				}
				if (wonder == 6) {
					Texture Maravilla(Models[0], "albedo", 0);
					for (auto& mesh : model.meshes) {
						mesh.textures.clear();
						mesh.textures.push_back(Maravilla);
					}
					inLimitedZone = true;
					ValuesOfTheLimitsOfWonder(6);
				}
				if (wonder == 7) {
					Texture Maravilla(Models[1], "albedo", 0);
					for (auto& mesh : model.meshes) {
						mesh.textures.clear();
						mesh.textures.push_back(Maravilla);
					}
					inLimitedZone = true;
					ValuesOfTheLimitsOfWonder(7);
				}
			}
			if (inLimitedZone) {
				cameras.Position.x = cameras.clamp(cameras.Position.x, minLimite.x, maxLimite.x);
				cameras.Position.y = cameras.clamp(cameras.Position.y, minLimite.y, maxLimite.y);
				cameras.Position.z = cameras.clamp(cameras.Position.z, minLimite.z, maxLimite.z);
			}

			if (flashlightOn) {
				shaderProgram_Scene.Activate();
				glUniform3f(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightColor"), Colors[index], Colors[index + 1], Colors[index + 2]);
				glUniform1f(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightIntensity"), flashlightIntensity);
				glUniform1i(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightOn"), flashlightOn);
				glUniform3fv(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightPos"), 1, glm::value_ptr(cameras.Position));
				glUniform3fv(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightDir"), 1, glm::value_ptr(cameras.Orientation));
				glUniform1f(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightCutOff"), flashlightCutOff);
				glUniform1f(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightOuterCutOff"), flashlightOuterCutOff);
			}
			else {
				flashlightCutOff = cos(glm::radians(12.1f));
				flashlightOuterCutOff = cos(glm::radians(17.1f));
				radieon = 1;
				index = 0;
				flashlightIntensity = 1.0f;
				shaderProgram_Scene.Activate();
				glUniform1f(glGetUniformLocation(shaderProgram_Scene.ID, "flashlightIntensity"), 0.0f);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram_Scene.Delete();
	return false;
}