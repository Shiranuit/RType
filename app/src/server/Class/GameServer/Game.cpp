/*
** EPITECH PROJECT, 2020
** B-CPP-501-MPL-5-1-rtype-antoine.maillard
** File description:
** Game
*/

#include "server/Class/GameServer/Game.hpp"
#include "server/Class/MessageHandlers/ServerSpawnMessageHandler.hpp"
#include "server/Class/MessageHandlers/ServerDeathMessageHandler.hpp"
#include "server/Class/MessageHandlers/ServerDamageMessageHandler.hpp"
#include "server/Class/MessageHandlers/ServerFireMessageHandler.hpp"
#include "server/Class/MessageHandlers/ServerMoveMessageHandler.hpp"
#include "server/Class/MessageHandlers/ServerPositionMessageHandler.hpp"

Game::Game():
    Synchronizer(),
    lobby(Ladder::genId()),
    gameServer(networkHandler)
{
    networkHandler.registerMessageHandler(new ServerSpawnMessageHandler(*this));
    networkHandler.registerMessageHandler(new ServerDeathMessageHandler(*this));
    networkHandler.registerMessageHandler(new ServerDamageMessageHandler(*this));
    networkHandler.registerMessageHandler(new ServerFireMessageHandler(*this));
    networkHandler.registerMessageHandler(new ServerMoveMessageHandler(*this));
    networkHandler.registerMessageHandler(new ServerPositionMessageHandler(*this));
}

Game::~Game()
{
}

void Game::init() {
    ecs.newEntityModel<Position, Velocity, EntityID, Hitbox, EntityInfo, EntityStats>("Player")
    .addTags({"Player", "Friendly", "Damageable"})
    .finish();

    ecs.newEntityModel<Position, Velocity, EntityID, Hitbox, EntityInfo, ProjectileInfo>("Projectile")
    .addTags({"DamageOnTouch", "Projectile"})
    .finish();

    ecs.newEntityModel<Position, Velocity, EntityID, Hitbox, EntityInfo, EntityStats>("Enemy")
    .addTags({"DamageOnTouch", "Damageable"})
    .finish();

    ecs.newEntityModel<Position, Velocity, EntityID, Hitbox>("Wall")
    .finish();

    ecs.newEntityModel<Position, Velocity, EntityID, Hitbox, EntityInfo, EntityStats>("DestructibleWall")
    .addTags({"DamageOnTouch", "Damageable"})
    .finish();

    ecs.newSystem<Velocity, EntityID, EntityInfo>("UpdateEntities")
    .each([this](float delta, EntityIterator<Velocity, EntityID, EntityInfo> &entity) {
        if (this->getDoubleMap().isReadClose())
            return;

        auto &readMap = this->getDoubleMap().getReadMap();
        while (entity.hasNext()) {
            entity.next();

            Velocity *velocity = entity.getComponent<Velocity>(0);
            EntityID *entityID = entity.getComponent<EntityID>(1);
            EntityInfo *entityInfo = entity.getComponent<EntityInfo>(2);

            if (readMap->find(entityID->id) != readMap->end()) {
                PacketData &data = readMap->at(entityID->id);

                velocity->dirX = data.dirX;
                velocity->dirY = data.dirY;

                entityInfo->isFiring = data.isFiring;
            }
        }
        this->getDoubleMap().closeRead();
    }).finish();

    ecs.newSystem<Position, EntityID, EntityInfo, Hitbox>("SpawnProjectile")
    .withoutTags({"Projectile"})
    .each([this](float delta, EntityIterator<Position, EntityID, EntityInfo, Hitbox> &entity) {
        auto projectileGenerator = this->getECS().getEntityGenerator("Projectile");
        while (entity.hasNext()) {
            entity.next();

            Position *position = entity.getComponent<Position>(0);
            EntityID *entityID = entity.getComponent<EntityID>(1);
            EntityInfo *entityInfo = entity.getComponent<EntityInfo>(2);
            Hitbox *hitbox = entity.getComponent<Hitbox>(3);

            if (entityInfo->isFiring) {
                bool isEnemy = entityInfo->isEnemy;

                projectileGenerator.instanciate(1,
                Position{isEnemy?position->x:position->x + hitbox->w, position->y + hitbox->h / 2},
                Velocity{isEnemy?-1:1, 0, 1000},
                EntityID{this->getNextEntityID()},
                ProjectileHitbox,
                EntityInfo{isEnemy, false},
                ProjectileInfo{20}
            );
            }
        }
    }).finish();

    ecs.newSystem<Position, Velocity, EntityID, EntityInfo, Hitbox>("MoveEntity")
    .each([this](float delta, EntityIterator<Position, Velocity, EntityID, EntityInfo, Hitbox> &entity) {
        while (entity.hasNext()) {
            entity.next();

            Position *position = entity.getComponent<Position>(0);
            Velocity *velocity = entity.getComponent<Velocity>(1);
            EntityID *entityID = entity.getComponent<EntityID>(2);
            EntityInfo *entityInfo = entity.getComponent<EntityInfo>(3);
            Hitbox *hitbox = entity.getComponent<Hitbox>(4);

            double moveX = velocity->dirX * velocity->speed * delta;
            double moveY = velocity->dirY * velocity->speed * delta;

            if (entityInfo->isEnemy) {
                if (position->x + hitbox->w + moveX < 0 || position->y + hitbox->h + moveY < 0 || position->y + moveY > this->getMapHeight()) {
                    this->getNetworkHandler().broadcast(DeathPacket(entityID->id));
                    entity.remove();
                    continue;
                }
            } else {
                if (position->x + hitbox->w + moveX < 0 || position->x + moveX > this->getMapWidth() ||
                    position->y + hitbox->h + moveY < 0 || position->y + moveY > this->getMapHeight()) {
                        continue;
                    }
            }

            position->x += moveX;
            position->y += moveY;
            this->getNetworkHandler().broadcast(PositionPacket(entityID->id, position->x, position->y));
        }
    }).finish();

    ecs.newSystem<Position, Hitbox, EntityInfo, EntityID, EntityStats>("ApplyDamage")
    .withTags({"Damageable"})
    .each([this](float delta, EntityIterator<Position, Hitbox, EntityInfo, EntityID, EntityStats> &entity) {
        auto projectileIterator = this->getECS().lookup<Position, Hitbox, EntityInfo, EntityID, ProjectileInfo>("Projectile");
        while (entity.hasNext()) {
            entity.next();

            Position *entityPosition = entity.getComponent<Position>(0);
            Hitbox *entityHitbox = entity.getComponent<Hitbox>(1);
            EntityInfo *entityInfo = entity.getComponent<EntityInfo>(2);
            EntityID *entityID = entity.getComponent<EntityID>(3);
            EntityStats *entityStats = entity.getComponent<EntityStats>(4);

            projectileIterator.reset();
            while (projectileIterator.hasNext()) {
                projectileIterator.next();

                EntityInfo *projectileEntityInfo = projectileIterator.getComponent<EntityInfo>(2);
                if ((!entityInfo->isEnemy && projectileEntityInfo->isEnemy) ||
                    (entityInfo->isEnemy && !projectileEntityInfo->isEnemy))
                {

                    Position *projectilePosition = projectileIterator.getComponent<Position>(0);
                    Hitbox *projectileHitbox = projectileIterator.getComponent<Hitbox>(1);
                    EntityID *projectileID = projectileIterator.getComponent<EntityID>(3);
                    ProjectileInfo *projectileInfo = projectileIterator.getComponent<ProjectileInfo>(4);

                    if (entityPosition->x < projectilePosition->x + projectileHitbox->w &&
                        entityPosition->x + entityHitbox->w > projectilePosition->x &&
                        entityPosition->y < projectilePosition->y + projectileHitbox->h &&
                        entityPosition->y + entityHitbox->h > projectilePosition->y)
                    {
                        entityStats->hp -= projectileInfo->damage;
                        this->getNetworkHandler().broadcast(DamagePacket(entityID->id, projectileInfo->damage, entityStats->hp, entityStats->maxHP));
                        this->getNetworkHandler().broadcast(DeathPacket(projectileID->id)); // Kill projectile
                        projectileIterator.remove();

                        if (entityStats->hp <= 0) {
                            this->getNetworkHandler().broadcast(DeathPacket(entityID->id)); // Kill Entity
                            entity.remove();
                            break;
                        }
                    }
                }

            }
        }
    }).finish();
}

void Game::loadExtensions(std::vector<std::unique_ptr<IExtension>> &extensions) {

}

void Game::compile() {
    ecs.compile();

    ecs.getEntityGenerator("DestructibleWall")
    .instanciate(1, Hitbox {200, getMapHeight()}, EntityInfo {true}, EntityStats{100, 100});

    ecs.getEntityGenerator("Projectile")
    .instanciate(1, Position{.x = getMapWidth(), .y = getMapHeight() / 2}, Velocity{.dirX = -1, .dirY = 0, .speed = 1000}, ProjectileInfo{.damage = 100}, Hitbox{.w = 50, .h = 50}, EntityInfo{false});
}

void Game::update() {
    auto now = std::chrono::high_resolution_clock::now();
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastTime);
    double delta = nanos.count() / 1000000000.0;

    lastTime = now;
    ecs.update(delta);
}

const server_info_t Game::setGameServer()
{
    server_info_t info;

    gameServer.configure();
    info.address = "127.0.0.1";
    info.port = gameServer.getPort();
    return (info);
}

void Game::startGame()
{
    gameServer.run();
    lastTime = std::chrono::high_resolution_clock::now();
    init();
    compile();
    while (true) {
        update();
    }
    gameServer.join();
}

Ladder &Game::getLobby()
{
    return (lobby);
}

ECS &Game::getECS() {
    return ecs;
}

NetworkHandler &Game::getNetworkHandler() {
    return networkHandler;
}