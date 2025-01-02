#pragma comment(lib, "Winmm.lib")

#include "Sage.h"

#include <codecvt>
#include <Windowsx.h>
#include <SpriteBatch.h>
#include "Audio.h"
#include "SageMath.h"
#include <DirectXColors.h>
#include <locale>
#include <random>
#include <sstream>

#include "Components/AnimatedSprite.h"
#include "Components/Behaviour.h"
#include "Components/Button.h"
#include "Utilities/Gizmos.h"
#include "../Zombies/Scenes/MainMenuScene.h"
#include "../Zombies/Scenes/GameScene.h"
#include "Components/PhysicsBody.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// Entry point for the application
int WINAPI WinMain(const HINSTANCE hInstance, HINSTANCE, PSTR, const int nShowCmd)
{
    Sage application(hInstance);

    if (application.InitWindowsApp(L"SAGE", nShowCmd) == false)
    {
        return 0;
    }

    if (application.InitializeDirect3D())
    {
        application.SetDepthStencil(true);
        application.InitializeTextures();
        application.MessageLoop();
    }

    return 0;
}

// Constructor for the application
Sage::Sage(const HINSTANCE hInstance)
    : DirectXClass(hInstance), gizmoText(), gizmoColour()
{
    mousePos = Vector2(clientWidth * 0.5f, clientHeight * 0.5f);
    spriteBatch = nullptr;

    clearColour = Color(Colors::DarkGray.v);
    isGamePaused = false;

    gizmoText = L"";
    gizmoColour = Colors::Magenta;
}

// Destructor for the application
Sage::~Sage()
{
    delete spriteBatch;
}

// Initialize the textures used in the game
// TODO: Function poorly named, cleanup needed
void Sage::InitializeTextures()
{
    harmoniaFont.InitializeFont(D3DDevice, deviceContext, L"..\\Font\\Harmonia.spritefont");
    spriteBatch = new SpriteBatch(deviceContext);
    Gizmos::sage = this;

    std::unique_ptr<Scene> mainMenuScene = std::make_unique<MainMenuScene>();
    mainMenuScene->Initialize(D3DDevice);

    std::unique_ptr<Scene> gameScene = std::make_unique<GameScene>();
    gameScene->Initialize(D3DDevice);

    scenes.push_back(std::move(mainMenuScene));
    scenes.push_back(std::move(gameScene));

    // Pass sage reference to scenes
    for (auto& scene : scenes)
    {
        scene->sage = this;
    }

    // Register listeners
    for (auto& gameObject : GetActiveScene()->gameObjects)
    {
        std::vector<Behaviour*> behaviours = gameObject->GetComponentsOfType<Behaviour>();
        for (auto* behaviour : behaviours)
        {
            keyboardListeners.push_back(behaviour);
            mouseListeners.push_back(behaviour);
            colliderListeners.push_back(behaviour);
            behaviour->Start();
        }
    }
}

// Process window messages
LRESULT Sage::ProcessWindowMessages(const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
    switch (msg)
    {
    case WM_MOUSEMOVE:
        mousePos.x = static_cast<float>(GET_X_LPARAM(lParam));
        mousePos.y = static_cast<float>(GET_Y_LPARAM(lParam));
        return 0;
    case WM_LBUTTONDOWN:
        leftButtonDown = true;
        if (pressedMouseButtons.find(1) == pressedMouseButtons.end()) // Button not already pressed
        {
            pressedMouseButtons.insert(1); // Mark as pressed
            for (auto* listener : mouseListeners)
                listener->OnButtonClick(1); // New call for button press
        }
        for (auto* listener : mouseListeners)
            listener->OnButtonDown(1);
        break;
    case WM_LBUTTONUP:
        leftButtonDown = false;
        pressedMouseButtons.erase(1); // Mark as released
        for (auto* listener : mouseListeners)
            listener->OnButtonUp(1);
        break;
    case WM_RBUTTONDOWN:
        if (pressedMouseButtons.find(2) == pressedMouseButtons.end()) // Button not already pressed
        {
            pressedMouseButtons.insert(2); // Mark as pressed
            for (auto* listener : mouseListeners)
                listener->OnButtonClick(2); // New call for button press
        }
        for (auto* listener : mouseListeners)
            listener->OnButtonDown(2);
        break;
    case WM_RBUTTONUP:
        pressedMouseButtons.erase(2); // Mark as released
        for (auto* listener : mouseListeners)
            listener->OnButtonUp(2);
    case WM_MBUTTONDOWN:
        if (pressedMouseButtons.find(3) == pressedMouseButtons.end()) // Button not already pressed
        {
            pressedMouseButtons.insert(3); // Mark as pressed
            for (auto* listener : mouseListeners)
                listener->OnButtonClick(3); // New call for button press
        }
        for (auto* listener : mouseListeners)
            listener->OnButtonDown(3);
        break;
    case WM_MBUTTONUP:
        pressedMouseButtons.erase(3); // Mark as released
        for (auto* listener : mouseListeners)
            listener->OnButtonUp(3);
        break;
    case WM_KEYDOWN:
        if (pressedKeys.find(wParam) == pressedKeys.end()) // Key not already pressed
        {
            pressedKeys.insert(wParam); // Mark as pressed
            for (auto* listener : keyboardListeners)
                listener->OnKeyPress(wParam); // New call for key press
        }
        for (auto* listener : keyboardListeners)
            listener->OnKeyDown(wParam);
        break;
    case WM_KEYUP:
        pressedKeys.erase(wParam); // Mark as released
        for (auto* listener : keyboardListeners)
            listener->OnKeyUp(wParam);
        break;
    }

    // Pass the message
    return DirectXClass::ProcessWindowMessages(msg, wParam, lParam);
}

// Render the game
void Sage::Render()
{
    spriteBatch->Begin(SpriteSortMode_BackToFront);

    // Draw the current scene
    for (const auto gameObject : GetActiveScene()->gameObjects)
    {
        if (!gameObject->IsRendered() || !gameObject->IsActive())
            continue;

        // Make sure a transform exists
        if (gameObject->GetComponent<Transform>() == nullptr)
            continue;

        const Sprite* sprite = gameObject->GetComponent<Sprite>();
        if (sprite != nullptr)
        {
            sprite->Draw(spriteBatch);
        }

        Button* button = gameObject->GetComponent<Button>();
        if (button != nullptr)
        {
            button->Draw(spriteBatch, mousePos, leftButtonDown);
        }

        AnimatedSprite* animatedSprite = gameObject->GetComponent<AnimatedSprite>();
        if (animatedSprite != nullptr)
        {
            animatedSprite->Draw(spriteBatch);
        }

        Collider* collider = gameObject->GetComponent<Collider>();
        if (collider != nullptr)
        {
            collider->Draw();
        }
    }

    spriteBatch->End();

    // Render text lines from bottom to top
    float baseY = Screen::GetHeight() - 40; // Start position for bottom line
    float lineSpacing = 30; // Space between lines
    
    for (size_t i = 0; i < gizmoTextLines.size(); i++) {
        const auto& line = gizmoTextLines[i];
        float y = baseY - (i * lineSpacing);

        // Calculate alpha based on remaining time
        float alpha = 1.0f;
        if (line.remainingTime <= 0) {
            alpha = 1.0f + (line.remainingTime / line.FADE_DURATION); // Linear fade from 1 to 0
            alpha = SageMath::Max(0.0f, SageMath::Min(1.0f, alpha)); // Clamp between 0 and 1
        }

        // Create a color with modified alpha
        DirectX::XMVECTORF32 fadeColor = line.color;
        fadeColor.f[3] = fadeColor.f[3] * alpha; // Modify the alpha component

        harmoniaFont.PrintMessage(75, y, line.text, fadeColor);
    }


    DirectXClass::UpdateScreenSize();
    deviceContext->VSSetConstantBuffers(0, 1, &screenSizeBuffer);

    // Draw the gizmo lines
    for (const auto& line : gizmoLines)
    {
        // Calculate the line direction and normal
        Vector2 lineDir = line.end - line.start;
        lineDir.Normalize();
        Vector2 normal(-lineDir.y, lineDir.x);
        normal *= line.thickness * 0.5f;

        // Create vertices for a rectangle
        DirectX::VertexPositionColor vertices[6];

        // First triangle
        vertices[0].position = DirectX::XMFLOAT3(line.start.x + normal.x, line.start.y + normal.y, 0.0f);
        vertices[1].position = DirectX::XMFLOAT3(line.start.x - normal.x, line.start.y - normal.y, 0.0f);
        vertices[2].position = DirectX::XMFLOAT3(line.end.x + normal.x, line.end.y + normal.y, 0.0f);

        // Second triangle
        vertices[3].position = DirectX::XMFLOAT3(line.end.x + normal.x, line.end.y + normal.y, 0.0f);
        vertices[4].position = DirectX::XMFLOAT3(line.start.x - normal.x, line.start.y - normal.y, 0.0f);
        vertices[5].position = DirectX::XMFLOAT3(line.end.x - normal.x, line.end.y - normal.y, 0.0f);

        // Set colors for all vertices
        for (int i = 0; i < 6; i++)
        {
            vertices[i].color = DirectX::XMFLOAT4(line.colour.f[0], line.colour.f[1],
                                                  line.colour.f[2], line.colour.f[3]);
        }

        // Update vertex buffer with rectangle vertices
        UpdateVertexBuffer(vertices, sizeof(vertices));

        // Set up the pipeline
        deviceContext->IASetInputLayout(inputLayout);
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        UINT stride = sizeof(DirectX::VertexPositionColor);
        UINT offset = 0;
        deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

        deviceContext->VSSetShader(vertexShader, nullptr, 0);
        deviceContext->PSSetShader(pixelShader, nullptr, 0);

        // Draw the triangles
        deviceContext->Draw(6, 0);
    }

    // render the base class
    DirectXClass::Render();
}

// Update the game
void Sage::Tick(float deltaTime)
{
    // First update physics bodies (movement)
    for (const auto gameObject : GetActiveScene()->gameObjects)
    {
        if (!gameObject->IsActive())
            continue;

        PhysicsBody* physicsBody = gameObject->GetComponent<PhysicsBody>();
        if (physicsBody != nullptr && (physicsBody->ContinuesWhenPaused() || !isGamePaused))
        {
            physicsBody->Tick(deltaTime);
            
            // Apply velocity to position
            Transform* transform = gameObject->GetComponent<Transform>();
            transform->SetPosition(transform->GetPosition() + physicsBody->GetVelocity());
        }

        // Update collider positions
        Collider* collider = gameObject->GetComponent<Collider>();
        if (collider != nullptr)
        {
            Transform* transform = gameObject->GetComponent<Transform>();
            if (transform != nullptr)
                collider->UpdatePosition(transform->GetPosition());
        }
    }

    // Then handle all collisions
    auto collisionPairs = DetectCollisions();
    
    for (const auto& pair : collisionPairs) 
    {
        Collider* colliderA = pair.objectA->GetComponent<Collider>();
        Collider* colliderB = pair.objectB->GetComponent<Collider>();
        
        // If either collider is a trigger, handle as trigger collision
        if (colliderA->IsTrigger() || colliderB->IsTrigger()) 
        {
            HandleTriggerCollision(pair);
        }
        // Otherwise handle as physical collision
        else 
        {
            HandlePhysicsCollision(pair, deltaTime);
        }
    }
    
    for (const auto gameObject : GetActiveScene()->gameObjects)
    {
        if (!gameObject->IsActive())
            continue;

        Sprite* sprite = gameObject->GetComponent<Sprite>();
        if (sprite != nullptr)
        {
            if (sprite->ContinuesWhenPaused() || !isGamePaused)
                sprite->UpdateAnimation(deltaTime);
        }

        Button* button = gameObject->GetComponent<Button>();
        if (button != nullptr)
        {
            if (button->ContinuesWhenPaused() || !isGamePaused)
                button->Tick(deltaTime);
        }

        AnimatedSprite* animatedSprite = gameObject->GetComponent<AnimatedSprite>();
        if (animatedSprite != nullptr)
        {
            if (animatedSprite->ContinuesWhenPaused() || !isGamePaused)
                animatedSprite->Tick(deltaTime);
        }

        std::vector<Behaviour*> behaviours = gameObject->GetComponentsOfType<Behaviour>();
        for (auto* behaviour : behaviours)
        {
            if (behaviour->ContinuesWhenPaused() || !isGamePaused)
                behaviour->Tick(deltaTime, mousePos);
        }
    }

    // Update gizmo lines
    for (auto it = gizmoTextLines.begin(); it != gizmoTextLines.end();) {
        it->remainingTime -= deltaTime;
        
        if (it->remainingTime <= -it->FADE_DURATION) {
            it = gizmoTextLines.erase(it);
        } else {
            ++it;
        }
    }
}

void Sage::ChangeScene(std::string sceneName)
{
    keyboardListeners.clear();
    mouseListeners.clear();

    for (auto& scene : scenes)
    {
        if (scene->name == sceneName)
            scene->isActive = true;
        else
            scene->isActive = false;
    }

    for (auto& gameObject : GetActiveScene()->gameObjects)
    {
        std::vector<Behaviour*> behaviours = gameObject->GetComponentsOfType<Behaviour>();
        for (auto* behaviour : behaviours)
        {
            keyboardListeners.push_back(behaviour);
            mouseListeners.push_back(behaviour);
            behaviour->Start();
        }
    }
}

void Sage::Pause()
{
    isGamePaused = true;
}

void Sage::Unpause()
{
    isGamePaused = false;
}

void Sage::WriteLine(const std::wstring& text, const DirectX::XMVECTORF32& color) 
{
    // Add new line at the end with full time
    gizmoTextLines.push_back({text, color, GizmoTextLine::FADE_TIME});
    
    // Remove oldest line if we exceed maximum
    if (gizmoTextLines.size() > MAX_TEXT_LINES) {
        gizmoTextLines.pop_front();
    }
}

std::vector<CollisionPair> Sage::DetectCollisions() 
{
    std::vector<CollisionPair> collisions;
    const auto& gameObjects = GetActiveScene()->gameObjects;

    for (size_t i = 0; i < gameObjects.size(); i++) 
    {
        GameObject* objectA = gameObjects[i];
        if (!objectA->IsActive())
            continue;

        Collider* colliderA = objectA->GetComponent<Collider>();
        if (!colliderA)
            continue;

        for (size_t j = i + 1; j < gameObjects.size(); j++) 
        {
            GameObject* objectB = gameObjects[j];
            if (!objectB->IsActive())
                continue;

            Collider* colliderB = objectB->GetComponent<Collider>();
            if (!colliderB)
                continue;

            // Check for collision
            if (colliderA->CheckCollision(*colliderB, Vector2::Zero))
            {
                CollisionPair pair;
                pair.objectA = objectA;
                pair.objectB = objectB;
                
                // Get detailed collision info
                if (colliderA->GetType() == RECTANGLE && colliderB->GetType() == RECTANGLE)
                    pair.result = colliderA->RectRectCheck(colliderA->GetSquare(), colliderB->GetSquare());
                else if (colliderA->GetType() == CIRCLE && colliderB->GetType() == CIRCLE)
                    pair.result = colliderA->CircleCircleCheck(colliderA->GetCircle(), colliderB->GetCircle());
                else if (colliderA->GetType() == RECTANGLE && colliderB->GetType() == CIRCLE)
                    pair.result = colliderA->RectCircleCheck(colliderA->GetSquare(), colliderB->GetCircle());
                else if (colliderA->GetType() == CIRCLE && colliderB->GetType() == RECTANGLE)
                    pair.result = colliderB->RectCircleCheck(colliderB->GetSquare(), colliderA->GetCircle());

                if (pair.result.collision)
                    collisions.push_back(pair);
            }
        }
    }
    
    return collisions;
}

void Sage::HandlePhysicsCollision(const CollisionPair& pair, float deltaTime)
{
    GameObject* objectA = pair.objectA;
    GameObject* objectB = pair.objectB;
    const CollisionResult& result = pair.result;

    // Notify collision listeners
    for (auto* behaviour : objectA->GetComponentsOfType<Behaviour>())
        behaviour->OnCollisionEnter(objectB);
    for (auto* behaviour : objectB->GetComponentsOfType<Behaviour>())
        behaviour->OnCollisionEnter(objectA);

    // Handle physics response
    PhysicsBody* physicsA = objectA->GetComponent<PhysicsBody>();
    PhysicsBody* physicsB = objectB->GetComponent<PhysicsBody>();
    
    // Calculate separation force
    float separationForce = 0.1f; // Small buffer to prevent sticking
    if (result.normal != Vector2::Zero) {
        Vector2 separation = result.normal * (result.penetrationDepth + separationForce);
        
        // If both objects have physics bodies, share the separation
        if (physicsA && physicsB) {
            separation *= 0.5f;
            objectA->GetComponent<Transform>()->SetPosition(
                objectA->GetComponent<Transform>()->GetPosition() + separation);
            objectB->GetComponent<Transform>()->SetPosition(
                objectB->GetComponent<Transform>()->GetPosition() - separation);
        }
        // Otherwise move just the object with physics
        else if (physicsA) {
            objectA->GetComponent<Transform>()->SetPosition(
                objectA->GetComponent<Transform>()->GetPosition() + separation);
        }
        else if (physicsB) {
            objectB->GetComponent<Transform>()->SetPosition(
                objectB->GetComponent<Transform>()->GetPosition() - separation);
        }
    }

    // Apply velocity changes
    if (physicsA) {
        Collider* colliderA = objectA->GetComponent<Collider>();
        Collider* colliderB = objectB->GetComponent<Collider>();
        physicsA->SetVelocity(colliderA->Reflect(*colliderB, physicsA->GetVelocity(), deltaTime));
    }
    if (physicsB) {
        Collider* colliderA = objectA->GetComponent<Collider>();
        Collider* colliderB = objectB->GetComponent<Collider>();
        physicsB->SetVelocity(colliderB->Reflect(*colliderA, physicsB->GetVelocity(), deltaTime));
    }
}

void Sage::HandleTriggerCollision(const CollisionPair& pair)
{
    GameObject* objectA = pair.objectA;
    GameObject* objectB = pair.objectB;

    // Only notify the non-trigger object about entering the trigger
    Collider* colliderA = objectA->GetComponent<Collider>();
    Collider* colliderB = objectB->GetComponent<Collider>();

    if (colliderA->IsTrigger()) {
        for (auto* behaviour : objectB->GetComponentsOfType<Behaviour>())
            behaviour->OnTriggerEnter(objectA);
    }
    if (colliderB->IsTrigger()) {
        for (auto* behaviour : objectA->GetComponentsOfType<Behaviour>())
            behaviour->OnTriggerEnter(objectB);
    }
}