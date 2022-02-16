//
//  Engineutils.h
//  VulkanEngine
//
//  Created by Gabriele Padovani on 20/01/22.
//

#ifndef EngineUtils_h
#define EngineUtils_h

#include <functional>

template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hashCombine(seed, rest), ...);
};

#endif /* Engineutils_h */
