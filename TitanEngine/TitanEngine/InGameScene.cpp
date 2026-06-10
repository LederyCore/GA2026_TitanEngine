#include "pch.h"
#include "InGameScene.h"

#include "Enemy.h"
#include "Player.h"

using namespace TitanEngine;

void InGameScene::OnLoad()
{
    GameObject* enemy = AddObject("Enemy");
    enemy->AddComponent<Enemy>();

    /*GameObject* player = AddObject("Player");
    player->AddComponent<Player>();*/
}

void InGameScene::OnUnLoad()
{

}
