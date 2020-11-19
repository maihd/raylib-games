#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t EcsEntity;
typedef uint64_t EcsComponent;

#define ECS_MASK(TYPES_COUNT, ...) TYPES_COUNT, (EcsComponent[]){ ##__VA_ARGS }

enum
{
    ECS_SYSTEM_UPDATE,
    ECS_SYSTEM_RENDER,
};

typedef struct EcsWorld EcsWorld;

typedef void (*EcsSystem)(EcsWorld* world);

EcsWorld*   EcsNew(int maxEntities, int componentCount, int systemCount);
void        EcsFree(EcsWorld** world);

void        EcsRegisterComponent(EcsWorld* world, EcsComponent component, int count, int size);
void        EcsRegisterSystem(EcsWorld* world, int type, EcsSystem system);

void        EcsRunSystemByType(EcsWorld* world, int type);
void        EcsRunSystemAtIndex(EcsWorld* world, int systemIndex);

void        EcsForCount(EcsWorld* world);
EcsEntity   EcsGetEntity(EcsWorld* world, int index);
EcsEntity   EcsNewEntity(EcsWorld* world);
void        EcsFreeEntity(EcsWorld* world, EcsEntity entity);

void*       EcsGetComponent(EcsWorld* world, EcsEntity entity, EcsComponent component);
bool        EcsHasComponent(EcsWorld* world, EcsEntity entity, EcsComponent component);
void        EcsAddComponent(EcsWorld* world, EcsEntity entity, EcsComponent component, void* data);
void        EcsRemoveComponent(EcsWorld* world, EcsEntity entity, EcsComponent component);

bool        EcsIsEntityValid(EcsWorld* world, EcsEntity entity);
int         EcsGetEntityLife(EcsWorld* world, EcsEntity entity);
void        EcsPrintEntity(EcsWorld* world, EcsEntity entity);
