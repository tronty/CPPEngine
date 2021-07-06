<a class="site-logo" href="http://www.moltengl.com/" title="Molten">
<img src="images/Molten-Logo-Banner.png" alt="Molten Home" style="width:256px;height:auto">
</a>

#What's New in Molten

Copyright (c) 2014-2017 [The Brenwill Workshop Ltd.](http://www.brenwill.com) All rights reserved.

*This document is written in [Markdown](http://en.wikipedia.org/wiki/Markdown) format. 
For best results, use a Markdown reader. You can also view the 
[formatted document](http://www.moltengl.com/docs/whats-new/molten-0.18.0-whats-new) online.*



Molten 0.18.0
-------------

Released 2017/06/20


#### MoltenVK

- Add dynamic library implementation for iOS.
- Support specialization function constants.
- Changes to `MVKDeviceConfiguration` API structure:
  - Add `debugMode` config setting to consolidate several behaviours suitable for 
    runtime debugging activities, including shader logging and GPU frame capture.
  - Remove `shaderConversionLogging` and `imageFlipY` config settings.
  - Hologram and ATW demos illustrate modifying MoltenVK config to enable
    MoltenVK performance tracking and debugging.
- Fix to `vkCmdClearAttachments()` when used from resubmitted command buffer.
- Allow descriptor binding to bind fewer resources than defined in descriptor set layout.
- Fix incorrect properties returned by `vkGetPhysicalDeviceFormatProperties()`
  for substitutable depth formats.
- MoltenShaderConverter don't indicate failure when estimated original GLSL cannot be built for logging.
- MoltenShaderConverter tool supports compute file extensions.
- Update to latest *Khronos* library dependencies.


Molten 0.17.0
-------------

Released 2017/05/03


#### MoltenVK

- Improved support for Retina and display contents scaling:
    - Always use `CAMetalLayer contentsScale` to determine `drawableSize` for swapchain images.
    - Remove `MVKDeviceConfiguration::supportDisplayContentsScale` configuration setting.
    - `VkSurfaceCapabilitiesKHR` now returns the same value for current, minimum & maximum extents.
- Fix issue where Vulkan image arrays were not fully copied to and from Vulkan buffers.
- Remove option to flip fragment coordinates on shader conversion.
- Include `vulkan.hpp` header in MoltenVK package to ensure version consistency with included `vulkan.h`.
- Update to latest *Khronos* library dependencies.


Molten 0.16.3
-------------

Released 2017/04/24


#### MoltenVK

- Fix crash when Vulkan command buffer destroyed immediately after queue idle.
- Fix crash when a pipeline is destroyed after the pipeline cache from which it was created.
- Fix incorrect surface format reported by `vkGetPhysicalDeviceSurfaceFormatsKHR()`.


Molten 0.16.2
-------------

Released 2017/03/30


#### MoltenVK

- Fix duplicate resource indexes when Descriptor Set Layout is reused.
- Support variation of available `MTLPixelFormats` per OS version.
- Add support for new Metal depth formats.
- Allow depth format substitution when required format unavailable.
- Add support for `VK_IMG_format_pvrtc` Vulkan extension and PVRT formats on iOS.


Molten 0.16.1
-------------

Released 2017/03/07


#### MoltenVK

- Pad and pack MSL shader uniform structure members to match SPIR-V structure member alignments.

- Add support for the following `VkImageViewCreateInfo::VkComponentMapping` per-texture swizzles:
	- `VK_FORMAT_R8_UNORM`:  `VkComponentMapping.r` = `VK_COMPONENT_SWIZZLE_R`
	- `VK_FORMAT_R8G8B8A8_UNORM` <->	`VK_FORMAT_B8G8R8A8_UNORM`
	- `VK_FORMAT_R8G8B8A8_SRGB` <->	 `VK_FORMAT_B8G8R8A8_SRGB`


Molten 0.16.0
-------------

Released 2017/02/20


#### MoltenVK

- Support extensions `VK_MVK_ios_surface` and `VK_MVK_macos_surface` embedded in `vulkan.h` file.
- Replace the `vk_mvk_ios_surface.h` and `vk_mvk_macos_surface.h` header files 
  with a single `mvk_vulkan.h` header file.
- Rename `vk_mvk_datatypes.h` header file to `mvk_datatypes.h`.
- Update to latest *LunarG* samples and demos.
- Fix to `Hologram` demo to use radians instead of degrees.


Molten 0.15.1
-------------

Released 2017/02/09


#### MoltenVK

- Support passing matrices as vertex/instance input to shaders.
- Support texel buffers on iOS.


Molten 0.15.0
-------------

Released 2017/01/25


#### MoltenVK

- Support image fetch (MTLTexture reads) in shaders.
- Remove unnecessary shader function declarations.
- Support writable buffers in shaders.


Molten 0.14.2
-------------

Released 2017/01/15


#### MoltenVK

- Improved support for compressed textures.
- Improved support for commands that can be called both inside and outside a renderpass.
- Improvements to occulsion queries.
- Improvements to accuracy of device features and limits reporting.
- Added support for vkCmdResolveImage().- Added support non-consecutive Vulkan descriptor binding numbers.
- Added support for SRC1 color blending.
- Improved resource management on El Capitan.
- SPIRV-MSL conversion logs partially converted MSL if conversion error occurs.
- SPRIV-MSL conversion simplified handling of vertex attributes.
- Add context labeling of MTLCommandBuffer, MTLRenderCommandEncoder, and MTLBlitCommandEncoder
  for identification during Xcode Metal GPU Frame Capture.
- Fix command buffer creation/completion deadlock.
- Fix minor renderpass attachment issues.


Molten 0.14.1
-------------

Released 2016/10/23


#### MoltenVK

- Shader converter handle missing names.


Molten 0.14.0
-------------

Released 2016/10/19


#### MoltenVK

- Clearing attachment sub-region with `vkCmdClearAttachments()` now aligned correctly.
- Update to latest Khronos lib dependencies.


Molten 0.13.3
-------------

Released 2016/10/13


#### MoltenGL

- Improved handling of retrieval of `CAMetalDrawable` from `CAMetalLayer`.


Molten 0.13.2
-------------

Released 2016/09/05


#### MoltenVK

- Add support for installing **MoltenVK** as a dynamic library on *macOS*.


Molten 0.13.1
-------------

Released 2016/08/31


#### MoltenVK

- *MSL* handles input structs originating from HLSL.
- Fix change in depth clip mode usage for *Metal* under *iOS 10.0*.


#### MoltenGL

- MSL only use half precision for GLSL low precision, and never for scalar floats.
- Support larger textures for GPUs and OS versions that support them.
- Replace GL_FORCE_RENDERABLE_TEXTURES_MGL capability with GL_OPTIMIZE_SAMPLE_ONLY_TEXTURES_MGL.


Molten 0.13.0
-------------

Released 2016/07/27


#### Initial public MoltenVK beta release!


Molten 0.12.0
-------------

Released 2016/05/31


#### Added MoltenVK

- Added MoltenVK
- MoltenGL renamed to Molten


MoltenGL 0.11.0
---------------

Released 2016/04/05

#### Renaming

- MetalGL renamed to MoltenGL


MetalGL 0.10.0
--------------

Released 2015/09/21

#### OS X Support

- Add support for running *OpenGL ES 2.0* applications under OS X

#### Sample Demo Projects

- Add `DrawLoadDemo` sample project to demonstrate **MoltenGL** vs *OpenGL* 
  performance characteristics.

#### GLKView

- Fix setting properties on iOS `MGLGLKView` via *Interface Builder*.

#### Misc

- Fixed issues related to several changes in *Metal* API under *iOS 9*
- Allow an *OpenGL* shader to be deleted immediately after being attached to an *OpenGL* program.
- Added support for *Xcode 7*
- `MetalGL_LICENSE.md` updated to clarify API license usage.


MetalGL 0.9.1
-------------

Released 2015/08/04

#### New OpenGL ES Extensions

- Add support for *OpenGL ES 2.0* `EXT_shadow_samplers` extension
- Add support for *OpenGL ES 2.0* `EXT_texture_filter_anisotropic` extension

#### Sample Demo Projects

- Add sample demo *Xcode* projects in `SampleProjects` folder
- Add `ParticlesDemo` sample project

#### Rendering Enhancements

- Add support for rendering to depth-only framebuffer

#### Shader Conversion Enhancements

- Convert empty *GLSL* shaders to *MSL*
- Trim whitespace from ends of *GLSL* & *MSL* source and shader info logs

#### Documentation Changes

- Rename `README.md` to `README-UserGuide.md`
- Add list of supported *OpenGL ES* extensions to `README-UserGuide.md`
- Add description of sample *Xcode* projects to `README-UserGuide.md`
- Add `SampleProjects/README-SampleProjects.md` to describe sample apps.
- Add `Whats_New.md` file to log changes delivered with each **MoltenGL** version


MetalGL 0.9.0
-------------

Released 2015/06/01

#### Initial public release!

