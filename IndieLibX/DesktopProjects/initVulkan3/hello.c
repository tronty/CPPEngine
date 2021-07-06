#include <Python.h>
#include "importvk.h"

#ifdef __IMPORTVK__
VkApplicationInfo applicationInfo;
VkInstanceCreateInfo instanceInfo;
VkInstance instance;
#endif

int vkSupported()
{
    int ret=0;
#ifndef __IMPORTVK__
	return 13;
#else
    if(importVKInit())
	return 2;

    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;	// sType
    applicationInfo.pNext = NULL;				// pNext
    applicationInfo.pApplicationName = "Blank Vulkan Window";	// Application name
    applicationInfo.pEngineName = NULL;				// Name of your graphics/game engine
    applicationInfo.engineVersion = 1;				// Engine development version
    applicationInfo.apiVersion = VK_API_VERSION_1_0;		// Vulkan's version

    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;// sType
    instanceInfo.pNext = NULL;					// pNext
    instanceInfo.flags = 0;					// flags, 0 for now
    instanceInfo.pApplicationInfo = &applicationInfo;		// link to vkApplicationInfo object
    instanceInfo.enabledLayerCount = 0;				// disable layers
    instanceInfo.ppEnabledLayerNames = NULL;
    instanceInfo.enabledExtensionCount = 0;			// disable extensions
    instanceInfo.ppEnabledExtensionNames = NULL;

    // Create the instance
    VkResult result = vkCreateInstance(&instanceInfo, NULL, &instance);

    // Check the result and exit the app with error code if not successful
    if (result != VK_SUCCESS) {
        //fprintf(stderr, "Cannot create VkInstance: %d\n", result);
	ret=3;
    } else {
        //printf("Successfully created VkInstance!\n");
	ret=0;
    }

    // ??? vkDestroyInstance(instance, NULL);				// release memory
#endif

    return ret;
}

static PyObject* helloworld(PyObject* self)
{
    return Py_BuildValue("s", "Hello from initVulkan3, Python extensions!!");
}

static PyObject* initVulkan(PyObject* self)
{
#ifdef __IMPORTVK__
    return Py_BuildValue("i", vkSupported());
#else
    return Py_BuildValue("i", 13);
#endif
}

static char initVulkan3_docs[] =
    "helloworld( ): Any message you want to put here!!\n";

static char initVulkan_docs[] =
    "initVulkan( ): Any message you want to put here!!\n";

static PyMethodDef initVulkan3_funcs[] = {
    {"helloworld", (PyCFunction)helloworld, 
     METH_NOARGS, initVulkan3_docs},
    {"initVulkan", (PyCFunction)initVulkan, 
     METH_NOARGS, initVulkan_docs},
    {NULL}
};

void initinitVulkan3(void)
{
    Py_InitModule3("initVulkan3", initVulkan3_funcs,
                   "Extension module example!");
}

