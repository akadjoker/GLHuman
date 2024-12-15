
#include "Core.hpp"
#include "Animation.hpp"

extern "C" const char* __lsan_default_suppressions() {
    return "leak:libSDL2\n"
           "leak:SDL_DBus\n";
}

int main(int argc, char *argv[])
{
    Device device;

    bool ABORT = false;

    if (!device.Init("HumanGL BY Luis Santos AKA DJOKER", 1024, 720))
    {
        return 1;
    }

    RenderBatch batch;
    batch.Init(1, 1024);
    Shader shader;
    Shader shaderCube;
    Font font;


    {
        const char *vShader = GLSL(
            layout(location = 0) in vec3 position;
            layout(location = 1) in vec2 texCoord;
            layout(location = 2) in vec4 color;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;

            out vec2 TexCoord;
            out vec4 vertexColor;
            void main() {
                gl_Position = projection * view * model * vec4(position, 1.0);
                TexCoord = texCoord;
                vertexColor = color;
            });

        const char *fShader = GLSL(
            in vec2 TexCoord;
            out vec4 color;
            in vec4 vertexColor;
            uniform sampler2D texture0;
            void main() {
                color = texture(texture0, TexCoord) * vertexColor;
            });

        shader.Create(vShader, fShader);
        shader.LoadDefaults();
    }

    {
        const char *vShader = GLSL(
            layout(location = 0) in vec3 position;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;

            void main() {
                gl_Position = projection * view * model * vec4(position, 1.0);
            });

        const char *fShader = GLSL(
            out vec4 color;
            uniform vec3 difusse;
            void main() {
                color = vec4(difusse, 1.0);
            });

        if (!shaderCube.Create(vShader, fShader))
        {
            ABORT = true;
        }
        shaderCube.LoadDefaults();
    }

    font.LoadDefaultFont();
    font.SetBatch(&batch);
    font.SetSize(12);

    Camera camera(800.0f / 600.0f);

    Driver::Instance().SetClearColor(0.2f, 0.3f, 0.3f, 1.0);

    float cameraSpeed = 0.5f;
    float mouseSensitivity = 0.1f;

    Humanoid human;

    bool CanAnimate = false;
GUI *widgets = GUI::Instance();
Window *window = widgets->CreateWindow("Mesh Transform", 2, 110, 360, 500);
window->SetVisible(false);

Window* keyframeEditor =widgets->CreateWindow("Animation Timeline", 2, 680, 900, 250);
keyframeEditor->CreateKeyframeEditor(5,5,885,200);


// Botões de animação existentes
Button *bWalk = window->CreateButton("Walk", 20, 10, 100, 20);
bWalk->SetkeyMap(true, SDLK_1);
bWalk->OnClick = [&]() 
{
    human.playAnimation("walk");
};

Button *bJump = window->CreateButton("Jump", 200, 10, 100, 20);
bJump->SetkeyMap(true, SDLK_2);
bJump->OnClick = [&]() 
{
    human.playAnimation("jump");
};

Button *bDance = window->CreateButton("Dance", 20, 40, 100, 20);
bDance->SetkeyMap(true, SDLK_3);
bDance->OnClick = [&]() 
{
    human.playAnimation("dance");
};

Button *bFight = window->CreateButton("Fight", 200, 40, 100, 20);
bFight->SetkeyMap(true, SDLK_4);
bFight->OnClick = [&]() 
{
    human.playAnimation("fight");
};


window->CreateLabel("Speed", 30, 75);
Slider *speed = window->CreateSlider(false, 80, 70, 180, 20, 0, 200, 100);


CheckBox *manualCheck =  window->CreateCheckBox("Manual Animation", false, 20, 120, 20, 20);
manualCheck->OnClick = [&]() 
{
   CanAnimate = manualCheck->GetState();
   if (CanAnimate)
   {
        human.reset();
   }
};





// Cabeça e Tronco
window->CreateLabel("Head", 20, 160);
Slider *headSlider = window->CreateSlider(false, 100, 160, 180, 20, -180, 180, 0);


window->CreateLabel("Torso", 20, 190);
Slider *torsoSlider = window->CreateSlider(false, 100, 190, 180, 20, -180, 180, 0);


// Braços 
window->CreateLabel("Arms", 150, 250);
window->CreateLabel("L", 70, 230);
window->CreateLabel("R", 255, 230);
Slider *leftArmSlider = window->CreateSlider(false, 20, 270, 120, 20, -180, 180, 0);
Slider *rightArmSlider = window->CreateSlider(false, 180, 270, 120, 20, -180, 180, 0);


// Antebraços 
window->CreateLabel("Forearms", 130, 300);

Slider *leftForearmSlider = window->CreateSlider(false, 20, 330, 120, 20, -180, 180, 0);
Slider *rightForearmSlider = window->CreateSlider(false, 180, 330, 120, 20, -180, 180, 0);

window->CreateLabel("Legs", 140, 360);

Slider *leftThighSlider = window->CreateSlider(false, 20, 390, 120, 20, -180, 180, 0);
Slider *rightThighSlider = window->CreateSlider(false, 180, 390, 120, 20, -180, 180, 0);


window->CreateLabel("Calves", 135, 420);

Slider *leftCalfSlider = window->CreateSlider(false, 20, 450, 120, 20, -180, 180, 0);
Slider *rightCalfSlider = window->CreateSlider(false, 180, 450, 120, 20, -180, 180, 0);



//Window* window2 = widgets->CreateWindow("Animation Timeline", 2, 620, 360, 200);
//WindowKeyframeEditor* keyframeEditor = window2->CreateKeyframeEditor( 2, 620, 360, 200);

//keyframeEditor->m_gui = widgets;
//widgets->Add(keyframeEditor);


    while (device.Running())
    {
        if (ABORT)
            break;

        float delta = device.GetFrameTime();
        widgets->Update(delta);
        

        if (Input::IsKeyDown(SDLK_w))
        {
            camera.move(cameraSpeed);
        }
        else if (Input::IsKeyDown(SDLK_s))
        {
            camera.move(-cameraSpeed);
        }

        if (Input::IsKeyDown(SDLK_a))
        {
            camera.strafe(-cameraSpeed);
        }
        else if (Input::IsKeyDown(SDLK_d))
        {
            camera.strafe(cameraSpeed);
        }

        if (Input::IsMouseButtonDown(SDL_BUTTON_LEFT) && !(widgets->Focus()))
        {
            int x = Input::GetMouseDeltaX();
            int y = Input::GetMouseDeltaY();
            camera.rotate(x, -y, mouseSensitivity);
        }

        Driver::Instance().SetBlend(false);
        Driver::Instance().SetDepthTest(true);
        Driver::Instance().SetDepthWrite(true);
        Driver::Instance().Clear();

        glViewport(0, 0, device.GetWidth(), device.GetHeight());

        Mat4 projection = camera.getProjectionMatrix();
        Mat4 view = camera.getViewMatrix();
        camera.setAspectRatio(device.GetWidth() / device.GetHeight());

        Mat4 identity = Mat4::Identity();

        // Render Scene

        // if (Input::IsKeyDown(SDLK_1))
        // {
        //     human.playAnimation("walk");
        // }
        // else if (Input::IsKeyDown(SDLK_2))
        // {
        //     human.playAnimation("jump");
        // }else if (Input::IsKeyDown(SDLK_3))
        // {
        //     human.playAnimation("dance");
        // }
        //  else if (Input::IsKeyDown(SDLK_4))
        // {
        //     human.playAnimation("fight");
        // }


        shaderCube.Use();
        shaderCube.SetMatrix4("model", identity.m);
        shaderCube.SetMatrix4("view", view.m);
        shaderCube.SetMatrix4("projection", projection.m);

     
        human.render(shaderCube);

   
        if (!CanAnimate)
        {
            human.animate(delta);
            human.setSpeed(speed->GetValue()/100.0f);
        } else 
        {
             
                human.setHeadRotation( headSlider->GetValue() * dtor);
                human.setTorsoRotation(torsoSlider->GetValue() * dtor);

                human.setUpperArmRotation(false,leftArmSlider->GetValue() * dtor);
                human.setUpperArmRotation(true,rightArmSlider->GetValue() * dtor);

                human.setForearmRotation(false,leftForearmSlider->GetValue() * dtor);
                human.setForearmRotation(true,rightForearmSlider->GetValue() * dtor);

                human.setThighRotation(false,leftThighSlider->GetValue() * dtor);
                human.setThighRotation(true,rightThighSlider->GetValue() * dtor);

                human.setCalfRotation(false,leftCalfSlider->GetValue() * dtor);
                human.setCalfRotation(true,rightCalfSlider->GetValue() * dtor);
                


        }
        // Render 3d Batch

        shader.Use();
        shader.SetMatrix4("model", identity.m);
        shader.SetMatrix4("view", view.m);
        shader.SetMatrix4("projection", projection.m);
        batch.SetColor(255, 255, 255, 255);

        batch.Grid(10, 10);
        batch.Render();

        // Render 2d STUFF

        projection = Mat4::Ortho(0.0f, device.GetWidth(), device.GetHeight(), 0.0f, -1.0f, 1.0f);
        Driver::Instance().SetBlend(true);
        Driver::Instance().SetBlendMode(BlendMode::BLEND);
        Driver::Instance().SetDepthTest(false);
        Driver::Instance().SetDepthWrite(false);

        shader.Use();
        shader.SetMatrix4("model", identity.m);
        shader.SetMatrix4("view", identity.m);
        shader.SetMatrix4("projection", projection.m);
        batch.SetColor(255, 255, 255, 255);

        widgets->Render(&batch);

        font.Print(10, 20, "FPS %d", device.GetFPS());

        batch.Render();

        device.Swap();
    }

    widgets->Clear();
    font.Release();
    batch.Release();
    shader.Release();
    shaderCube.Release();

    device.Close();
    return 0;
}