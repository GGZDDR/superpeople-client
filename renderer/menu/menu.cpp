#include "menu.hpp"

#include <array>
#include <functional>

#include "../renderer.hpp"

#include "../../engine/engine.hpp"

#include "../../dependencies/rawdata/person.hpp"
#include "../../dependencies/rawdata/skeleton.hpp"

#include "../../dependencies/libs/imgui_tricks.hpp"

#include "../../modules/aimbot/aimbot.hpp"

#include "config/config.hpp"

static auto current_tab = 0;

void Draw_Main( float, float )
{
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0,0 } );

    ImGui::SetCursorPos( { 24, 235 } );
    ImGui::BeginChild( xs( "Main" ), { 337, 530 }, false, ImGuiWindowFlags_NoScrollbar );
    {
        switch ( current_tab )
        {
        case 0:
             renderer::imgui::elements::functions::checkbox( xs( "Aimbot" ), xs( "enables aimbot" ), std::get< bool >( menu::config::config_map[ "aimbot_enable" ] ) );

            renderer::imgui::elements::functions::hotkey( "##1", &std::get< int >( menu::config::config_map[ "aimbot_hotkey" ] ) );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Ignore Teammates" ), xs( "ignores teammates" ), std::get< bool >( menu::config::config_map[ "aimbot_ignore_team" ] ) );

            renderer::imgui::elements::functions::separator( );
            
            renderer::imgui::elements::functions::information( xs( "Smoothness" ), xs( "sets the smoothness of the aimbot" ), std::get< int >( menu::config::config_map[ "aimbot_smoothness" ] ), "%i" );

            renderer::imgui::elements::functions::slider_int( "##2", &std::get< int >( menu::config::config_map[ "aimbot_smoothness" ] ), 0, 10, "" );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::information( xs( "Bone" ), xs( "assigns the aiming bone" ), std::get< int >( menu::config::config_map[ "aimbot_bone" ] ), "%i" );

            renderer::imgui::elements::functions::combo( "##3", &std::get< int >( menu::config::config_map[ "aimbot_bone" ] ), "head\0\rneck\0\rchest\0\0", 1 );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::information( xs( "Max Distance" ), xs( "sets the distance interval to aim at" ), std::get< int >( menu::config::config_map[ "aimbot_max_distance" ] ), "%i" );

            renderer::imgui::elements::functions::slider_int( "##4", &std::get< int >( menu::config::config_map[ "aimbot_max_distance" ] ), 0, 300, "" );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Show FOV" ), xs( "shows circular FOV" ), std::get< bool >( menu::config::config_map[ "aimbot_show_fov" ] ) );

            renderer::imgui::elements::functions::information( xs( "FOV" ), xs( "sets the aimbot fov" ), std::get< int >( menu::config::config_map[ "aimbot_fov" ] ), "%i" );

            renderer::imgui::elements::functions::slider_int( "##5", &std::get< int >( menu::config::config_map[ "aimbot_fov" ] ), 0, 100, "" );

            renderer::imgui::elements::functions::color_picker( "##6", std::get< engine::structs::vector4_t >( menu::config::config_map[ "fov_color" ] ).color );

            renderer::imgui::elements::functions::information( xs( "FOV Segments" ), xs( "adjusts the aimbot fov shape" ), std::get< int >( menu::config::config_map[ "aimbot_fov_segments" ] ), "%i" );

            renderer::imgui::elements::functions::slider_int( "##7", &std::get< int >( menu::config::config_map[ "aimbot_fov_segments" ] ), 0, 10, "" );
            break;
        case 1:          
            renderer::imgui::elements::functions::checkbox( xs( "Trigger Bot" ), xs( "enables the triggerbot" ), std::get< bool >( menu::config::config_map[ "triggerbot_enable" ] ) );

            renderer::imgui::elements::functions::hotkey( "##1", &std::get< int >( menu::config::config_map[ "triggerbot_hotkey" ] ) );
            break;
        case 2:                    
            renderer::imgui::elements::functions::checkbox( xs( "No Recoil" ), xs( "removes recoil from the weapon equipped" ), std::get< bool >( menu::config::config_map[ "enable_no_recoil" ] ) );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Fast Shoot" ), xs( "projectiles increase in speed making you shoot faster" ), std::get< bool >( menu::config::config_map[ "enable_fast_shoot" ] ) );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Fast ADS" ), xs( "increases the speed of hip to ads" ), std::get< bool >( menu::config::config_map[ "enable_fast_ads" ] ) );
            break;
        case 3:
            renderer::imgui::elements::functions::information( xs( "Max Distance" ), xs( "sets the max visual distance" ), std::get< int >( menu::config::config_map[ "visuals_max_distance" ] ), "%i" );

            renderer::imgui::elements::functions::slider_int( "##1", &std::get< int >( menu::config::config_map[ "visuals_max_distance" ] ), 0, 300, "" );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Ignore Teammates" ), xs( "ignores teammates" ), std::get< bool >( menu::config::config_map[ "visuals_ignore_team" ] ) );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Bounding Box" ), xs( "draws a rectangular box on entities" ), std::get< bool >( menu::config::config_map[ "visuals_bounding_box" ] ) );

            renderer::imgui::elements::functions::color_picker( "##2", std::get< engine::structs::vector4_t >( menu::config::config_map[ "bounding_box_color" ] ).color );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Filled Box" ), xs( "draws a filled rectangular box on entities" ), std::get< bool >( menu::config::config_map[ "visuals_filled_box" ] ) );

            renderer::imgui::elements::functions::color_picker( "##3", std::get< engine::structs::vector4_t >( menu::config::config_map[ "filled_box_color" ] ).color );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Corner Box" ), xs( "draws a cornered box on entities" ), std::get< bool >( menu::config::config_map[ "visuals_corner_box" ] ) );

            renderer::imgui::elements::functions::color_picker( "##4", std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Head Hitbox" ), xs( "draws a head square on entities" ), std::get< bool >( menu::config::config_map[ "visuals_head_hitbox" ] ) );

            renderer::imgui::elements::functions::color_picker( "##5", std::get< engine::structs::vector4_t >( menu::config::config_map[ "hitbox_color" ] ).color );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Skeleton" ), xs( "draws a skeleton on entities" ), std::get< bool >( menu::config::config_map[ "visuals_skeleton" ] ) );

            renderer::imgui::elements::functions::color_picker( "##6", std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Distance" ), xs( "draws distance of entity" ), std::get< bool >( menu::config::config_map[ "visuals_distance" ] ) );

            renderer::imgui::elements::functions::color_picker( "##7", std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Health" ), xs ( "draws health of entity" ), std::get< bool >( menu::config::config_map[ "visuals_health" ] ) );

            renderer::imgui::elements::functions::color_picker( "##8", std::get< engine::structs::vector4_t >( menu::config::config_map[ "health_color" ] ).color );
            break;
        case 4:
            renderer::imgui::elements::functions::button( xs( "Save Config" ), { 328, 35 }, renderer::imgui::elements::button_flags::BUTTON_PURPLE );

            ImGui::NewLine( );

            renderer::imgui::elements::functions::button( xs( "Load Config" ), { 328, 35 }, renderer::imgui::elements::button_flags::BUTTON_GRAY );
            break;
        default:
            break;
        }
    }
    ImGui::EndChild( );

    ImGui::PopStyleVar( );
}

void renderer::menu::draw( )
{
    modules::aimbot::draw_fov( );

    if ( renderer::values::renderer->is_menu_open )
    {
        ImGui::SetNextWindowSize( { 375, 770 } );

        ImGui::Begin( xs( "Squad" ), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar );
        {
            auto DrawList = ImGui::GetWindowDrawList( );
            auto Position = ImGui::GetWindowPos( );

            DrawList->AddText( renderer::imgui::elements::fonts::druk_wide_bold, 30, { Position.x + 24, Position.y + 36 }, ImColor{ 255, 255, 255 }, xs( "Squad" ) );

            DrawList->AddRectFilled( { Position.x + 24, Position.y + 73 }, { Position.x + 24 + 187, Position.y + 73 + 2 }, ImColor{ 255, 255, 255, 10 } );
            DrawList->AddRectFilled( { Position.x + 24, Position.y + 73 }, { Position.x + 24 + 88, Position.y + 73 + 2 }, ImColor{ 127, 128, 246 } );

            ImGui::SetCursorPos( { 24, 107 } );

            ImGui::BeginGroup( );
            {
                renderer::imgui::elements::functions::tab( xs( "LEGIT" ), ICON_FA_SHIELD_CHECK, current_tab, 0 );

                ImGui::SameLine( );

                renderer::imgui::elements::functions::tab( xs( "RAGE" ), ICON_FA_SHIELD, current_tab, 1 );

                ImGui::SameLine( );

                renderer::imgui::elements::functions::tab( xs( "MISC" ), ICON_FA_FOLDER, current_tab, 2 );

                ImGui::SameLine( );

                renderer::imgui::elements::functions::tab( xs( "VISUALS" ), ICON_FA_BOLT, current_tab, 3 );

                ImGui::SameLine( );

                renderer::imgui::elements::functions::tab( xs( "CFG" ), ICON_FA_ARCHIVE, current_tab, 4 );

            }
            ImGui::EndGroup( );

            Draw_Main( 0.f, 0.f );
        }
        ImGui::End( );
    }
    else
    {
        ImGui::Begin( xs( "UnrealWindow" ), nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_HorizontalScrollbar ); 
        { 
            auto DrawList = ImGui::GetWindowDrawList( );

            ImGui::SetWindowSize( { 220, 40 } );

            ImGui::SetWindowPos( { 0, 0 } );

            DrawList->AddText( renderer::imgui::elements::fonts::druk_wide_bold, 25, { 15, 5 }, ImColor { 255, 255, 255, 70 }, xs( "loaded" ) );
        }
        ImGui::End( );
    }
}