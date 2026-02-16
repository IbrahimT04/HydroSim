//
// Created by itari on 2026-02-14.
//
#include "scene.h"

Scene::Scene() {
    for (float x = -1.0f; x < 1.0f; x += 0.2f) {
        for (float y = -1.0f; y < 1.0f; y += 0.2f) {
            trianglePositions.emplace_back(x, y, 0.0f);
        }
    }
    // constexpr int  count  = 10;
    // for (int ix = 0; ix < count; ++ix) {
    //     constexpr float step  = 0.2f;
    //     constexpr float start = -1.0f;
    //     float x = start + static_cast<float>(ix) * step;
    //     for (int iy = 0; iy < count; ++iy) {
    //         float y = start + static_cast<float>(iy) * step;
    //         trianglePositions.emplace_back(x, y, 0.0f);
    //     }
    // }
}


