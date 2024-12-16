#pragma once

#include <string>
#include <map>
#include "Core.hpp"

struct Vertex
{
    float position[3];
};



struct Pose
{
    Vec3 position;
    float torsoRotation;
    float headRotation;
    float upperArmRotation[2]; // [0] = esquerdo, [1] = direito
    float forearmRotation[2];  // [0] = esquerdo, [1] = direito
    float thighRotation[2];    // [0] = esquerdo, [1] = direito
    float calfRotation[2];     // [0] = esquerdo, [1] = direito

    Pose()
    {
        torsoRotation = 0.0f;
        headRotation = 0.0f;
        for (int i = 0; i < 2; i++)
        {
            upperArmRotation[i] = 0.0f;
            forearmRotation[i] = 0.0f;
            thighRotation[i] = 0.0f;
            calfRotation[i] = 0.0f;
        }
    }

    static Pose lerp(const Pose &a, const Pose &b, float t)
    {
        Pose result;
        result.torsoRotation = a.torsoRotation * (1 - t) + b.torsoRotation * t;
        result.headRotation = a.headRotation * (1 - t) + b.headRotation * t;
        result.position.x = a.position.x  * (1 - t) + b.position.x * t;
        result.position.y = a.position.y  * (1 - t) + b.position.y * t;
        result.position.z = a.position.z  * (1 - t) + b.position.z * t;

        for (int i = 0; i < 2; i++)
        {
            result.upperArmRotation[i] = a.upperArmRotation[i] * (1 - t) + b.upperArmRotation[i] * t;
            result.forearmRotation[i] = a.forearmRotation[i] * (1 - t) + b.forearmRotation[i] * t;
            result.thighRotation[i] = a.thighRotation[i] * (1 - t) + b.thighRotation[i] * t;
            result.calfRotation[i] = a.calfRotation[i] * (1 - t) + b.calfRotation[i] * t;
        }
        return result;
    }
};

struct Keyframe
{
    Pose pose;
    float time; // Tempo em segundos
};

class Animation
{
public:
    std::string name;
    std::vector<Keyframe> keyframes;
    bool loop;
    float duration;

    Animation()
    {
    }

    Animation(const std::string &name, bool loop = true)
        : name(name), loop(loop), duration(0.0f) {}

    void addKeyframe(const Pose &pose, float time)
    {
        Keyframe kf = {pose, time};
        keyframes.push_back(kf);
        duration = std::max(duration, time);
    }

    void clear()
    {
        keyframes.clear();
    }
    
    Pose getPoseAtTime(float time);
};

class AnimationManager
{
private:
    std::map<std::string, Animation> animations;
    std::string currentAnimation;
    std::string nextAnimation;
    float currentTime;
    float playbackSpeed;
    bool playing;
    bool looping;

public:
    AnimationManager() : currentTime(0.0f),
                         playbackSpeed(1.0f),
                         playing(false),
                         looping(true) {}

    void reset()
    {
        currentTime = 0.0f;
        playing = false;
        currentAnimation = "";
        nextAnimation = "";
    }

    std::string const CurrentAnimation() {return currentAnimation;}

    void addAnimation(const Animation &anim)
    {
        animations[anim.name] = anim;
    }

    
    Animation* getAnimation(const std::string name);

    void playAnimation(const std::string &name, bool shouldLoop = true, bool resetTime = true);

    void update(float deltaTime);

    void setPlaybackSpeed(float speed) { playbackSpeed = speed; }
    float getPlaybackSpeed() const { return playbackSpeed; }

    void pause() { playing = false; }
    void resume() { playing = true; }
    bool isPlaying() const { return playing; }

    float getCurrentTime() const { return currentTime; }
    void setCurrentTime(float time) { currentTime = time; }

    const std::string &getCurrentAnimationName() const { return currentAnimation; }
    

    bool isLooping() const { return looping; }
    void setLooping(bool loop) { looping = loop; }

    Pose getCurrentPose();
    void createDanceAnimation();
    void createFighterAnimation();
    void createDefaultAnimations();
    Pose getDefaultPose();

};

class Humanoid
{
private:
    MeshBuffer *cubeMesh;
    MeshBuffer *sphereMesh;
    MatrixStack matrixStack;
    Vec3 position;

    AnimationManager animManager;

    // Ângulos de rotação para cada parte
    float torsoRotation = 0.0f;
    float headRotation = 0.0f;

    // Braços
    float upperArmRotation[2] = {0.0f, 0.0f}; // [0] = esquerdo, [1] = direito
    float forearmRotation[2] = {0.0f, 0.0f};  // [0] = esquerdo, [1] = direito

    // Pernas
    float thighRotation[2] = {0.0f, 0.0f}; // [0] = esquerda, [1] = direita
    float calfRotation[2] = {0.0f, 0.0f};  // [0] = esquerda, [1] = direita

public:
    Humanoid();

    ~Humanoid();
   
    void reset();


    AnimationManager& GetAnimationManager() { return animManager;}
    

    void render(Shader &shader);
    
     float getTorsoRotation() const { return torsoRotation; }
    float getHeadRotation() const { return headRotation; }
    
    float getUpperArmRotation(bool isRight) const {return upperArmRotation[isRight ? 1 : 0];}
    
    float getForearmRotation(bool isRight) const {return forearmRotation[isRight ? 1 : 0];}
    
    float getThighRotation(bool isRight) const {return thighRotation[isRight ? 1 : 0];}
    
    float getCalfRotation(bool isRight) const {return calfRotation[isRight ? 1 : 0];}

    Vec3 getPosition() const { return position; }
    void setPosition(const Vec3& pos) { position = pos; }


    void setTorsoRotation(float angle) { torsoRotation = angle; }
    void setHeadRotation(float angle) { headRotation = angle; }

    void setUpperArmRotation(bool isRight, float angle)
    {
        upperArmRotation[isRight ? 1 : 0] = angle;
    }

    void setForearmRotation(bool isRight, float angle)
    {
        forearmRotation[isRight ? 1 : 0] = angle;
    }

    void setThighRotation(bool isRight, float angle)
    {
        thighRotation[isRight ? 1 : 0] = angle;
    }

    void setCalfRotation(bool isRight, float angle)
    {
        calfRotation[isRight ? 1 : 0] = angle;
    }

    void animate(float deltaTime);
   
    void playAnimation(const std::string &name)
    {
        animManager.playAnimation(name);
    }

    void setSpeed(float s)
    {
        animManager.setPlaybackSpeed(s);
    }

private:
    void renderCube(Shader &shader, const Mat4 &modelMatrix, const Color &color)
    {
        shader.SetMatrix4("model", modelMatrix.m);
        float r = color.r / 255.0f;
        float g = color.g / 255.0f;
        float b = color.b / 255.0f;

        shader.SetFloat("difusse", r, g, b);
        cubeMesh->Render(static_cast<int>(PrimitiveType::TRIANGLES), 36);
    }
    void renderSphere(Shader &shader, const Mat4 &modelMatrix, const Color &color)
    {
        shader.SetMatrix4("model", modelMatrix.m);
        float r = color.r / 255.0f;
        float g = color.g / 255.0f;
        float b = color.b / 255.0f;

        shader.SetFloat("difusse", r, g, b);
        sphereMesh->Render(static_cast<int>(PrimitiveType::TRIANGLES), 36);
    }
};



class AnimationTimeline : public Widget 
{
private:
    std::map<float, Pose> keyframes;
    float selectedTime;
    float gridWidth;
    float gridHeight;
    int divisions;
    Pose currentPose;
    
public:
    AnimationTimeline(float x, float y, float width, float height) 
        : Widget(), selectedTime(0), divisions(10) 
    {
        m_position.x = x;
        m_position.y = y;
        m_size.x = width;
        m_size.y = height;
        gridWidth = width / divisions;
        gridHeight = height;
        
    }
    void SetPose(Pose &pose)
    {
        currentPose = pose;
    }
    void OnUpdate(float delta) override
    {
        m_bounds = Rectangle(GetRealX(),GetRealY(), m_size.x+1, m_size.y+1);
        
    }
    void OnDraw(RenderBatch* batch) override 
    {
        Skin * skin = GUI::Instance()->GetSkin();
        Font * font = skin->GetFont();
        font->SetSize(10);
        batch->DrawRectangle(GetRealX()-(gridWidth/2)-8, GetRealY()-15, m_size.x+(gridWidth)+21, m_size.y+(gridWidth/2)+2,(m_focus)?Color(208,108,108):Color(108,108,108), false);
        batch->DrawRectangle(GetRealX()-(gridWidth/2)-5, GetRealY(), m_size.x+(gridWidth)+15, m_size.y,  Color(128,128,128), true);

        for(int i = 0; i <= divisions; i++) 
        {
            float x = GetRealX() + (i * gridWidth);

    
            font->DrawText(batch, x-2 , GetRealY()-11 ,skin->GetColor(LABEL),"%d",i);


            float time = i / (float)divisions;
            if(keyframes.find(time) != keyframes.end())
             {
                batch->DrawCircle(x, GetRealY() + gridHeight/2, 5, Color(0,0,255), true);
            }
            // Linha vertical
            batch->DrawRectangle(x, GetRealY(), 2, m_size.y, Color(64,64,64), true);


        }
        font->SetSize(14);

        // Desenha playhead no cursor
        float playheadX = GetRealX() + (selectedTime * m_size.x);
        batch->DrawRectangle((playheadX-1), GetRealY(), 2, m_size.y,  Color(255,0,0), true);
    }

    void OnMouseDown(int x, int y, int button) override 
    {
         if(button == 1 && IsInside(x,y) ) 
        {
            float relativeX = x - GetRealX();
            float time = relativeX / m_size.x;
            time = std::max(0.0f, std::min(1.0f, time));
            
            // Arredona para a divisão mais próxima
            time = roundf(time * divisions) / divisions;
            
            selectedTime = time;
            
            // add/remove
            if(keyframes.find(time) != keyframes.end()) 
            {
                keyframes.erase(time);
            } else 
            {
                
                
                keyframes[time] = currentPose;
            }
        }
    }
    void OnMouseMove(int x, int y)
    {
        m_focus = m_bounds.Contains(x, y);
        
    }



    void Clear()
    {
        keyframes.clear();
    }

    float GetSelectedTime() const { return selectedTime; }
    bool HasKeyframe(float time) const { return keyframes.find(time) != keyframes.end(); }
    const Pose* GetKeyframe(float time) const 
    {
        auto it = keyframes.find(time);
        return it != keyframes.end() ? &it->second : nullptr;
    }
};
