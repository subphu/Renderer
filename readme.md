# Installation

## Used Libraries
- Vulkan
- SDL2
- GLM

## Windows
- Install VulkanSDK
  - In the Select Components, check `GLM headers` and `SDL2 libraries and headers` component to install

## Mac
- Install VulkanSDK then copy and rename `macOS` folder inside VulkanSDK to `libraries/vulkansdk-mac`
    - In `libraries/vulkansdk-mac/lib`, rename `libvulkan.1.2.182.dylib` to `libvulkan.1.dylib`
