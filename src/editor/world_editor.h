// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FPL_WORLD_EDITOR_H_
#define FPL_WORLD_EDITOR_H_

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include "camera.h"
#include "fplbase/asset_manager.h"
#include "components_generated.h"
#include "components/physics.h"
#include "components/rendermesh.h"
#include "components/transform.h"
#include "components/editor.h"
#include "entity/component_interface.h"
#include "entity/entity_manager.h"
#include "fplbase/input.h"
#include "fplbase/utilities.h"
#include "mathfu/vector_3.h"
#include "world_editor_generated.h"

#ifdef __ANDROID__
#include "inputcontrollers/android_cardboard_controller.h"
#else
#include "inputcontrollers/mouse_controller.h"
#endif

namespace fpl {
namespace editor {

class WorldEditor {
 public:
  void Initialize(const WorldEditorConfig* config, InputSystem* input_system,
                  entity::EntityManager* entity_manager);
  void AdvanceFrame(WorldTime delta_time);
  void Render(Renderer* renderer);
  void Activate();
  void Deactivate();
  void SetInitialCamera(const fpl_project::Camera& initial_camera);

  void RegisterComponent(entity::ComponentInterface* component);

  const fpl_project::Camera* GetCamera() const { return &camera_; }

  void SelectEntity(const entity::EntityRef& entity_ref);

  void SaveWorld();
  void SaveEntitiesInFile(const std::string& filename);

 private:
  enum { kMoving, kEditing } input_mode_;

  // get camera movement via W-A-S-D
  mathfu::vec3 GetMovement();

  flatbuffers::Offset<EntityDef> SerializeEntity(
      entity::EntityRef& entity, flatbuffers::FlatBufferBuilder* builder,
      const reflection::Schema* schema);

  entity::EntityRef DuplicateEntity(entity::EntityRef& entity);
  void DestroyEntity(entity::EntityRef& entity);
  void HighlightEntity(const entity::EntityRef& entity, float tint);
  void NotifyEntityUpdated(const entity::EntityRef& entity) const;
  void NotifyEntityDeleted(const entity::EntityRef& entity) const;

  // returns true if the transform was modified
  bool ModifyTransformBasedOnInput(TransformDef* transform);

  void LoadSchemaFiles();

  const WorldEditorConfig* config_;
  InputSystem* input_system_;
  entity::EntityManager* entity_manager_;
  // Which entity are we currently editing?
  entity::EntityRef selected_entity_;

  // Temporary solution to let us cycle through all entities.
  std::unique_ptr<entity::EntityManager::EntityStorageContainer::Iterator>
      entity_cycler_;

  // For storing the FlatBuffers schema we use for exporting.
  std::string schema_data_;
  std::string schema_text_;

  bool previous_relative_mouse_mode;
#ifdef __ANDROID__
  fpl_project::AndroidCardboardController input_controller_;
#else
  fpl_project::MouseController input_controller_;
#endif
  fpl_project::Camera camera_;
};

}  // namespace editor
}  // namespace fpl

#endif  // FPL_WORLD_EDITOR_H_
