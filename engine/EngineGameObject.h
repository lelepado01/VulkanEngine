//
//  EngineGameObject.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 17/01/22.
//

#ifndef EngineGameObject_h
#define EngineGameObject_h

#include <stdio.h>
#include <memory>
#include <unordered_map>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "EngineModel.h"

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    glm::vec3 rotation{};
    
    glm::mat4 mat4();
    glm::mat3 normalMatrix(); 
};

enum GameObjectTag {
    SIMPLE,
    TERRAIN,
    INSTANCED,
    Number
};

class EngineGameObject {
 public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, EngineGameObject>;
    GameObjectTag tag;

  static EngineGameObject createGameObject() {
    static id_t currentId = 0;
    return EngineGameObject{currentId++};
  }

    EngineGameObject(const EngineGameObject &) = delete;
    EngineGameObject &operator=(const EngineGameObject &) = delete;
    EngineGameObject(EngineGameObject &&) = default;
    EngineGameObject &operator=(EngineGameObject &&) = default;

  id_t getId() { return id; }

  TransformComponent transform{};

  // Optional pointer components
  std::shared_ptr<EngineModel> model{};

 private:
    EngineGameObject(id_t objId) : id{objId} {}

  id_t id;
};

#endif /* EngineGameObject_h */
