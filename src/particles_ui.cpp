#include "particles_ui.hpp"


void DrawParticleConfigUI(ParticleConfig &cfg, const char *tag)
{
    static Vector2 window_size = {350, 400};
    ImGui::SetNextWindowSize({window_size.x, window_size.y}, ImGuiCond_Always);
    ImGui::Begin((std::string("Particle Config Wizard - ") + tag).c_str(), nullptr, ImGuiWindowFlags_NoResize);
    ImGui::SetWindowFontScale(1);
    
    auto normalized_rgb_to_rlcol = [](float norm_col[4]){
        return Color{
            .r = (uint8_t)std::clamp(norm_col[0] * 255.0f, 0.0f, 255.0f),
            .g = (uint8_t)std::clamp(norm_col[1] * 255.0f, 0.0f, 255.0f),
            .b = (uint8_t)std::clamp(norm_col[2] * 255.0f, 0.0f, 255.0f),
            .a = (uint8_t)std::clamp(norm_col[3] * 255.0f, 0.0f, 255.0f),
        };
    };

    {
        ImGui::TextColored({1,1,1,1}, "Start RGB");
        float startcol[4] = {
            cfg.start_colour.r / 255.0f,
            cfg.start_colour.g / 255.0f,
            cfg.start_colour.b / 255.0f,
            cfg.start_colour.a / 255.0f
        };
        ImGui::ColorPicker4("##StartRGB", &startcol[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB);
        cfg.start_colour = normalized_rgb_to_rlcol(startcol);
    }

    ImGui::Separator();
    
    {
        ImGui::TextColored({1,1,1,1}, "End RGB");
        float endcol[4] = {
            cfg.target_colour.r / 255.0f,
            cfg.target_colour.g / 255.0f,
            cfg.target_colour.b / 255.0f,
            cfg.target_colour.a / 255.0f
        };
        ImGui::ColorPicker4("##EndRGB", &endcol[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB);
        cfg.target_colour = normalized_rgb_to_rlcol(endcol);
    }

    ImGui::Separator();

    ImGui::TextColored({1,1,1,1}, "Acceleration");
    ImGui::DragFloat("Accel val x", &cfg.acceleration.value.x, 1, -10000.0f, 10000.0f);
    ImGui::DragFloat("Accel val y", &cfg.acceleration.value.y, 1, -10000.0f, 10000.0f);
    ImGui::DragFloat("Accel ± x", &cfg.acceleration.variation.x, 1, -10000.0f, 10000.0f);
    ImGui::DragFloat("Accel ± y", &cfg.acceleration.variation.y, 1, -10000.0f, 10000.0f);
    
    ImGui::Separator();

    ImGui::Checkbox("Blending ", &cfg.blending);

    ImGui::Separator();
    
    ImGui::TextColored({1,1,1,1}, "Emission");
    ImGui::DragFloat("##Emision", &cfg.emission, 0.1f, 0.0f, 10000.0f);
    
    ImGui::Separator();
    
    ImGui::TextColored({1,1,1,1}, "Fade In");
    ImGui::DragFloat("Fade In val", &cfg.fade_in.value, 0.005, 0, 1);
    ImGui::DragFloat("Fade In ±", &cfg.fade_in.variation, 0.005, 0, 1);
    
    ImGui::Separator();
    
    ImGui::TextColored({1,1,1,1}, "Fade Out");
    ImGui::DragFloat("Fade Out val", &cfg.fade_out.value, 0.005, 0, 1);
    ImGui::DragFloat("Fade Out ±", &cfg.fade_out.variation, 0.005, 0, 1);
    
    ImGui::Separator();
    
    ImGui::TextColored({1,1,1,1}, "Start Speed");
    ImGui::DragFloat("Start Vel x", &cfg.initial_velocity.value.x, 0.05, -10000, 10000);
    ImGui::DragFloat("Start Vel ± x", &cfg.initial_velocity.variation.x, 0.05, -10000, 10000);
    ImGui::DragFloat("Start Vel y", &cfg.initial_velocity.value.y, 0.05, -10000, 10000);
    ImGui::DragFloat("Start Vel ± y", &cfg.initial_velocity.variation.y, 0.05, -10000, 10000);
    
    ImGui::Separator();
    
    ImGui::TextColored({1,1,1,1}, "Lifetime");
    ImGui::DragFloat("Lifetime Val", &cfg.lifetime.value, 0.05, 0, 36000);  // <- Max 10hrs of lifetime. That should be enough... right?
    ImGui::DragFloat("Lifetime ±", &cfg.lifetime.variation, 0.05, 0, 36000);
    
    ImGui::Separator();
    
    ImGui::TextColored({1,1,1,1}, "Position Variation");
    ImGui::DragFloat("Pos Var x", &cfg.position_var.x, 0.05, -10000, 10000);
    ImGui::DragFloat("Pos Var y", &cfg.position_var.y, 0.05, -10000, 10000);
    
    ImGui::Separator();
    
    // Helper lambda
    auto rgbsync = [](Color *c, const char *tag_prefix)
    {
        int r = c->r;
        int g = c->g;
        int b = c->b;
        int a = c->a;
        ImGui::DragInt((std::string(tag_prefix) + "Red").c_str(), &r, 1, 0, 255);
        ImGui::DragInt((std::string(tag_prefix) + "Green").c_str(), &g, 1, 0, 255);
        ImGui::DragInt((std::string(tag_prefix) + "Blue").c_str(), &b, 1, 0, 255);
        ImGui::DragInt((std::string(tag_prefix) + "Alpha").c_str(), &a, 1, 0, 255);
        c->r = r;
        c->g = g;
        c->b = b;
        c->a = a;
    };
    ImGui::TextColored({1,1,1,1}, "Start Tint Variation");
    rgbsync(&cfg.rgb_var_start, "Start");
    ImGui::TextColored({1,1,1,1}, "End Tint Variation");
    rgbsync(&cfg.rgb_var_end, "End");
    
    ImGui::Separator();
    
    ImGui::TextColored({1,1,1,1}, "Start Rotation");
    ImGui::DragFloat("Start Rotation Val", &cfg.start_rotation.value, 0.05, -10000, 10000);
    ImGui::DragFloat("Start Rotation ±", &cfg.start_rotation.variation, 0.05, -10000, 10000);
    
    ImGui::Separator();
    
    ImGui::TextColored({1,1,1,1}, "End Rotation");
    ImGui::DragFloat("End Rotation Val", &cfg.target_rotation.value, 0.05, -10000, 10000);
    ImGui::DragFloat("End Rotation ±", &cfg.target_rotation.variation, 0.05, -10000, 10000);
    
    ImGui::Separator();

    ImGui::TextColored({1,1,1,1}, "Start Scale");
    ImGui::DragFloat("Start Scale x", &cfg.start_scale.value.x, 0.05, -10000, 10000);
    ImGui::DragFloat("Start Scale ± x", &cfg.start_scale.variation.x, 0.05, -10000, 10000);
    ImGui::DragFloat("Start Scale y", &cfg.start_scale.value.y, 0.05, -10000, 10000);
    ImGui::DragFloat("Start Scale ± y", &cfg.start_scale.variation.y, 0.05, -10000, 10000);
    
    ImGui::Separator();

    ImGui::TextColored({1,1,1,1}, "End Scale");
    ImGui::DragFloat("End Scale x", &cfg.target_scale.value.x, 0.05, -1000, 1000);
    ImGui::DragFloat("End Scale ± x", &cfg.target_scale.variation.x, 0.05, -1000, 1000);
    ImGui::DragFloat("End Scale y", &cfg.target_scale.value.y, 0.05, -1000, 1000);
    ImGui::DragFloat("End Scale ± y", &cfg.target_scale.variation.y, 0.05, -1000, 1000);

    ImGui::Separator();

    if (ImGui::Button("Copy to clipboard", {ImGui::GetContentRegionAvail().x, 50})){
        std::string config = "";
        config = config + 
        "{\n" +
        "   .acceleration = {\n" +
        "       .value = {" + std::to_string(cfg.acceleration.value.x) + "," + std::to_string(cfg.acceleration.value.y) + "},\n" +
        "       .variation = {" + std::to_string(cfg.acceleration.variation.x) + "," + std::to_string(cfg.acceleration.variation.y) + "}\n" +
        "   },\n" +
        "   .blending = " + ((cfg.blending == true) ? "true" : "false") + ",\n" +
        "   .emission = " + std::to_string(cfg.emission) + ",\n" +
        "   .fade_in = {\n" +
        "       .value = " + std::to_string(cfg.fade_in.value) + ",\n" + 
        "       .variation = " + std::to_string(cfg.fade_in.variation) + "\n" + 
        "   },\n" + 
        "   .fade_out = {\n" +
        "       .value = " + std::to_string(cfg.fade_out.value) + ",\n" + 
        "       .variation = " + std::to_string(cfg.fade_out.variation) + "\n" + 
        "   },\n" + 
        "   .initial_velocity = {\n" +
        "       .value = {" + std::to_string(cfg.initial_velocity.value.x) + "," + std::to_string(cfg.initial_velocity.value.y) + "},\n" +
        "       .variation = {" + std::to_string(cfg.initial_velocity.variation.x) + "," + std::to_string(cfg.initial_velocity.variation.y) + "}\n" +
        "   },\n" +
        "   .lifetime = {\n" +
        "       .value = " + std::to_string(cfg.lifetime.value) + ",\n" + 
        "       .variation = " + std::to_string(cfg.lifetime.variation) + "\n" + 
        "   },\n" + 
        "   .MAX_PARTICLES = " + std::to_string(cfg.MAX_PARTICLES) + ",\n" +
        "   .position_var = {" + std::to_string(cfg.position_var.x) + "," + std::to_string(cfg.position_var.y) + "},\n" +
        "   .rgb_var_end = {\n" +
        "       .r = " + std::to_string(cfg.rgb_var_end.r) + ",\n"
        "       .g = " + std::to_string(cfg.rgb_var_end.g) + ",\n"
        "       .b = " + std::to_string(cfg.rgb_var_end.b) + ",\n"
        "       .a = " + std::to_string(cfg.rgb_var_end.a) + ",\n"
        "   },\n"
        "   .rgb_var_start = {\n" +
        "       .r = " + std::to_string(cfg.rgb_var_start.r) + ",\n"
        "       .g = " + std::to_string(cfg.rgb_var_start.g) + ",\n"
        "       .b = " + std::to_string(cfg.rgb_var_start.b) + ",\n"
        "       .a = " + std::to_string(cfg.rgb_var_start.a) + ",\n"
        "   },\n"
        "   .start_colour = {\n" +
        "       .r = " + std::to_string(cfg.start_colour.r) + ",\n"
        "       .g = " + std::to_string(cfg.start_colour.g) + ",\n"
        "       .b = " + std::to_string(cfg.start_colour.b) + ",\n"
        "       .a = " + std::to_string(cfg.start_colour.a) + ",\n"
        "   },\n"
        "   .start_rotation = {\n" +
        "       .value = " + std::to_string(cfg.start_rotation.value) + ",\n" + 
        "       .variation = " + std::to_string(cfg.start_rotation.variation) + "\n" + 
        "   },\n" + 
        "   .start_scale = {\n" +
        "       .value = {" + std::to_string(cfg.start_scale.value.x) + "," + std::to_string(cfg.start_scale.value.y) + "},\n" +
        "       .variation = {" + std::to_string(cfg.start_scale.variation.x) + "," + std::to_string(cfg.start_scale.variation.y) + "}\n" +
        "   },\n" +
        "   .target_colour = {\n" +
        "       .r = " + std::to_string(cfg.target_colour.r) + ",\n"
        "       .g = " + std::to_string(cfg.target_colour.g) + ",\n"
        "       .b = " + std::to_string(cfg.target_colour.b) + ",\n"
        "       .a = " + std::to_string(cfg.target_colour.a) + ",\n"
        "   },\n"
        "   .target_rotation = {\n" +
        "       .value = " + std::to_string(cfg.target_rotation.value) + ",\n" + 
        "       .variation = " + std::to_string(cfg.target_rotation.variation) + "\n" + 
        "   },\n" + 
        "   .target_scale = {\n" +
        "       .value = {" + std::to_string(cfg.target_scale.value.x) + "," + std::to_string(cfg.start_scale.value.y) + "},\n" +
        "       .variation = {" + std::to_string(cfg.target_scale.variation.x) + "," + std::to_string(cfg.start_scale.variation.y) + "}\n" +
        "   },\n" +
        "};\n";


        SetClipboardText(config.c_str());
        //std::cout << "Particle config exported!\n ";
    }

    ImGui::SetWindowFontScale(1);
    ImGui::End();
}
