//
// DebugUI.cpp - ImGui binding.
// Copyright (C) Sebastian Nordgren 
// October 6th 2015
//

#include "PCH.h"
#include "DebugUI.h"

// GL3W/GLFW
#include "OpenGL.h"
#include "GLFW/glfw3.h"
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

#include "File.h"
#include "GLError.h"
#include "IDebugPanel.h"
#include "GLFWInputSource.h"
#include "Window.h"


// Data
static GLFWwindow*  g_Window = NULL;
static double       g_Time = 0.0f;
static bool         g_MouseJustPressed[3] = { false, false, false };
static float        g_MouseWheel = 0.0f;
static GLuint       g_FontTexture = 0;
static int			g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;

namespace dd
{
	// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
	// If text or lines are blurry when integrating ImGui in your engine:
	// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
	void DebugUI::RenderDrawLists( ImDrawData* draw_data )
	{
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		ImGuiIO& io = ImGui::GetIO();
		int fb_width = (int) (io.DisplaySize.x * io.DisplayFramebufferScale.x);
		int fb_height = (int) (io.DisplaySize.y * io.DisplayFramebufferScale.y);
		if( fb_width == 0 || fb_height == 0 )
			return;
		draw_data->ScaleClipRects( io.DisplayFramebufferScale );

		// Backup GL state
		GLenum last_active_texture; glGetIntegerv( GL_ACTIVE_TEXTURE, (GLint*) &last_active_texture );
		glActiveTexture( GL_TEXTURE0 );
		GLint last_program; glGetIntegerv( GL_CURRENT_PROGRAM, &last_program );
		GLint last_texture; glGetIntegerv( GL_TEXTURE_BINDING_2D, &last_texture );
		GLint last_sampler; glGetIntegerv( GL_SAMPLER_BINDING, &last_sampler );
		GLint last_array_buffer; glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &last_array_buffer );
		GLint last_element_array_buffer; glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer );
		GLint last_vertex_array; glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &last_vertex_array );
		GLint last_polygon_mode[2]; glGetIntegerv( GL_POLYGON_MODE, last_polygon_mode );
		GLint last_viewport[4]; glGetIntegerv( GL_VIEWPORT, last_viewport );
		GLint last_scissor_box[4]; glGetIntegerv( GL_SCISSOR_BOX, last_scissor_box );
		GLenum last_blend_src_rgb; glGetIntegerv( GL_BLEND_SRC_RGB, (GLint*) &last_blend_src_rgb );
		GLenum last_blend_dst_rgb; glGetIntegerv( GL_BLEND_DST_RGB, (GLint*) &last_blend_dst_rgb );
		GLenum last_blend_src_alpha; glGetIntegerv( GL_BLEND_SRC_ALPHA, (GLint*) &last_blend_src_alpha );
		GLenum last_blend_dst_alpha; glGetIntegerv( GL_BLEND_DST_ALPHA, (GLint*) &last_blend_dst_alpha );
		GLenum last_blend_equation_rgb; glGetIntegerv( GL_BLEND_EQUATION_RGB, (GLint*) &last_blend_equation_rgb );
		GLenum last_blend_equation_alpha; glGetIntegerv( GL_BLEND_EQUATION_ALPHA, (GLint*) &last_blend_equation_alpha );
		GLboolean last_enable_blend = glIsEnabled( GL_BLEND );
		GLboolean last_enable_cull_face = glIsEnabled( GL_CULL_FACE );
		GLboolean last_enable_depth_test = glIsEnabled( GL_DEPTH_TEST );
		GLboolean last_enable_scissor_test = glIsEnabled( GL_SCISSOR_TEST );

		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
		glEnable( GL_BLEND );
		glBlendEquation( GL_FUNC_ADD );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glDisable( GL_CULL_FACE );
		glDisable( GL_DEPTH_TEST );
		glEnable( GL_SCISSOR_TEST );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

		// Setup viewport, orthographic projection matrix
		glViewport( 0, 0, (GLsizei) fb_width, (GLsizei) fb_height );
		const float ortho_projection[4][4] =
		{
			{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
			{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
			{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
			{ -1.0f,                  1.0f,                   0.0f, 1.0f },
		};
		glUseProgram(g_ShaderHandle );
		glUniform1i( g_AttribLocationTex, 0 );
		glUniformMatrix4fv( g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0] );
		glBindVertexArray( g_VaoHandle );
		glBindSampler( 0, 0 ); // Rely on combined texture/sampler state.

		for( int n = 0; n < draw_data->CmdListsCount; n++ )
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawIdx* idx_buffer_offset = 0;

			glBindBuffer( GL_ARRAY_BUFFER, g_VboHandle );
			glBufferData( GL_ARRAY_BUFFER, (GLsizeiptr) cmd_list->VtxBuffer.Size * sizeof( ImDrawVert ), (const GLvoid*) cmd_list->VtxBuffer.Data, GL_STREAM_DRAW );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) cmd_list->IdxBuffer.Size * sizeof( ImDrawIdx ), (const GLvoid*) cmd_list->IdxBuffer.Data, GL_STREAM_DRAW );

			for( int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++ )
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if( pcmd->UserCallback )
				{
					pcmd->UserCallback( cmd_list, pcmd );
				}
				else
				{
					glBindTexture( GL_TEXTURE_2D, (GLuint) (intptr_t) pcmd->TextureId );
					glScissor( (int) pcmd->ClipRect.x, (int) (fb_height - pcmd->ClipRect.w), (int) (pcmd->ClipRect.z - pcmd->ClipRect.x), (int) (pcmd->ClipRect.w - pcmd->ClipRect.y) );
					glDrawElements( GL_TRIANGLES, (GLsizei) pcmd->ElemCount, sizeof( ImDrawIdx ) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset );
				}
				idx_buffer_offset += pcmd->ElemCount;
			}
		}

		// Restore modified GL state
		glUseProgram( last_program );
		glBindTexture( GL_TEXTURE_2D, last_texture );
		glBindSampler( 0, last_sampler );
		glActiveTexture( last_active_texture );
		glBindVertexArray( last_vertex_array );
		glBindBuffer( GL_ARRAY_BUFFER, last_array_buffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer );
		glBlendEquationSeparate( last_blend_equation_rgb, last_blend_equation_alpha );
		glBlendFuncSeparate( last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha );
		if( last_enable_blend ) glEnable( GL_BLEND ); else glDisable( GL_BLEND );
		if( last_enable_cull_face ) glEnable( GL_CULL_FACE ); else glDisable( GL_CULL_FACE );
		if( last_enable_depth_test ) glEnable( GL_DEPTH_TEST ); else glDisable( GL_DEPTH_TEST );
		if( last_enable_scissor_test ) glEnable( GL_SCISSOR_TEST ); else glDisable( GL_SCISSOR_TEST );
		glPolygonMode( GL_FRONT_AND_BACK, last_polygon_mode[0] );
		glViewport( last_viewport[0], last_viewport[1], (GLsizei) last_viewport[2], (GLsizei) last_viewport[3] );
		glScissor( last_scissor_box[0], last_scissor_box[1], (GLsizei) last_scissor_box[2], (GLsizei) last_scissor_box[3] );

		DiscardOGLError();
	}

	static const char* GetClipboardText( void* data )
	{
		return glfwGetClipboardString( g_Window );
	}

	static void SetClipboardText( void* data, const char* text )
	{
		glfwSetClipboardString( g_Window, text );
	}

	void DebugUI::MouseButtonCallback( GLFWwindow*, int button, int action, int /*mods*/ )
	{
		if( action == GLFW_PRESS && button >= 0 && button < 3 )
			g_MouseJustPressed[button] = true;
	}

	void DebugUI::ScrollCallback( GLFWwindow*, double /*xoffset*/, double yoffset )
	{
		g_MouseWheel += (float) yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
	}

	void DebugUI::KeyCallback( GLFWwindow*, int key, int, int action, int mods )
	{
		ImGuiIO& io = ImGui::GetIO();
		if( action == GLFW_PRESS )
			io.KeysDown[key] = true;
		if( action == GLFW_RELEASE )
			io.KeysDown[key] = false;

		(void) mods; // Modifiers are not reliable across systems
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	}

	void DebugUI::CharCallback( GLFWwindow*, unsigned int c )
	{
		ImGuiIO& io = ImGui::GetIO();
		if( c > 0 && c < 0x10000 )
			io.AddInputCharacter( (unsigned short) c );
	}

	void DebugUI::CreateFontsTexture()
	{
		// Build texture atlas
		ImGuiIO& io = ImGui::GetIO();
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

																	// Upload texture to graphics system
		GLint last_texture;
		glGetIntegerv( GL_TEXTURE_BINDING_2D, &last_texture );
		glGenTextures( 1, &g_FontTexture );
		glBindTexture( GL_TEXTURE_2D, g_FontTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

		// Store our identifier
		io.Fonts->TexID = (void *) (intptr_t) g_FontTexture;

		// Restore state
		glBindTexture( GL_TEXTURE_2D, last_texture );
	}

	bool DebugUI::CreateDeviceObjects()
	{
		// Backup GL state
		GLint last_texture, last_array_buffer, last_vertex_array;
		glGetIntegerv( GL_TEXTURE_BINDING_2D, &last_texture );
		glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &last_array_buffer );
		glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &last_vertex_array );

		const GLchar *vertex_shader =
			"#version 150\n"
			"uniform mat4 ProjMtx;\n"
			"in vec2 Position;\n"
			"in vec2 UV;\n"
			"in vec4 Color;\n"
			"out vec2 Frag_UV;\n"
			"out vec4 Frag_Color;\n"
			"void main()\n"
			"{\n"
			"	Frag_UV = UV;\n"
			"	Frag_Color = Color;\n"
			"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
			"}\n";

		const GLchar* fragment_shader =
			"#version 150\n"
			"uniform sampler2D Texture;\n"
			"in vec2 Frag_UV;\n"
			"in vec4 Frag_Color;\n"
			"out vec4 Out_Color;\n"
			"void main()\n"
			"{\n"
			"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
			"}\n";

		g_ShaderHandle = glCreateProgram();
		g_VertHandle = glCreateShader( GL_VERTEX_SHADER );
		g_FragHandle = glCreateShader( GL_FRAGMENT_SHADER );
		glShaderSource( g_VertHandle, 1, &vertex_shader, 0 );
		glShaderSource( g_FragHandle, 1, &fragment_shader, 0 );
		glCompileShader( g_VertHandle );
		glCompileShader( g_FragHandle );
		glAttachShader( g_ShaderHandle, g_VertHandle );
		glAttachShader( g_ShaderHandle, g_FragHandle );
		glLinkProgram( g_ShaderHandle );

		g_AttribLocationTex = glGetUniformLocation( g_ShaderHandle, "Texture" );
		g_AttribLocationProjMtx = glGetUniformLocation( g_ShaderHandle, "ProjMtx" );
		g_AttribLocationPosition = glGetAttribLocation( g_ShaderHandle, "Position" );
		g_AttribLocationUV = glGetAttribLocation( g_ShaderHandle, "UV" );
		g_AttribLocationColor = glGetAttribLocation( g_ShaderHandle, "Color" );

		glGenBuffers( 1, &g_VboHandle );
		glGenBuffers( 1, &g_ElementsHandle );

		glGenVertexArrays( 1, &g_VaoHandle );
		glBindVertexArray( g_VaoHandle );
		glBindBuffer( GL_ARRAY_BUFFER, g_VboHandle );
		glEnableVertexAttribArray( g_AttribLocationPosition );
		glEnableVertexAttribArray( g_AttribLocationUV );
		glEnableVertexAttribArray( g_AttribLocationColor );

		glVertexAttribPointer( g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof( ImDrawVert ), (GLvoid*) IM_OFFSETOF( ImDrawVert, pos ) );
		glVertexAttribPointer( g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof( ImDrawVert ), (GLvoid*) IM_OFFSETOF( ImDrawVert, uv ) );
		glVertexAttribPointer( g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( ImDrawVert ), (GLvoid*) IM_OFFSETOF( ImDrawVert, col ) );

		CreateFontsTexture();

		// Restore modified GL state
		glBindTexture( GL_TEXTURE_2D, last_texture );
		glBindBuffer( GL_ARRAY_BUFFER, last_array_buffer );
		glBindVertexArray( last_vertex_array );

		return true;
	}

	DebugUI::DebugUI( Window& window, GLFWInputSource& input )
	{
		m_input = &input;
		m_window = &window;

		m_input->AddKeyboardCallback( &DebugUI::KeyCallback );
		m_input->AddScrollCallback( &DebugUI::ScrollCallback );
		m_input->AddMouseCallback( &DebugUI::MouseButtonCallback );
		m_input->AddCharCallback( &DebugUI::CharCallback );

		g_Window = window.GetInternalWindow();

		ImGuiIO& io = ImGui::GetIO();
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;


		File file = File( "fonts\\Roboto-Medium.ttf" );

		size_t size = file.Size();
		Buffer<byte> buffer(new byte[size], size);
		size_t read = file.Read( buffer );

		io.Fonts->AddFontFromMemoryTTF( buffer.Access(), (int) read, 16.0f );

		//io.Fonts->AddFontDefault();

		io.RenderDrawListsFn = &DebugUI::RenderDrawLists; // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
		io.SetClipboardTextFn = SetClipboardText;
		io.GetClipboardTextFn = GetClipboardText;

#ifdef _WIN32
		io.ImeWindowHandle = glfwGetWin32Window( g_Window );
#endif

		CreateDeviceObjects();
	}

	DebugUI::~DebugUI()
	{
		if( g_VaoHandle != 0 )
			glDeleteVertexArrays( 1, &g_VaoHandle );

		if( g_VboHandle != 0 )
			glDeleteBuffers( 1, &g_VboHandle );

		if( g_ElementsHandle != 0 )
			glDeleteBuffers( 1, &g_ElementsHandle );

		g_VaoHandle = g_VboHandle = g_ElementsHandle = 0;

		glDetachShader( g_ShaderHandle, g_VertHandle );
		if( g_VertHandle != 0 ) glDeleteShader( g_VertHandle );
		g_VertHandle = 0;

		glDetachShader( g_ShaderHandle, g_FragHandle );
		if( g_FragHandle != 0 ) glDeleteShader( g_FragHandle );
		g_FragHandle = 0;

		if(g_ShaderHandle != 0 ) glDeleteProgram( g_ShaderHandle );
		g_ShaderHandle = 0;

		if( g_FontTexture )
		{
			glDeleteTextures( 1, &g_FontTexture );
			ImGui::GetIO().Fonts->TexID = 0;
			g_FontTexture = 0;
		}
		ImGui::Shutdown();
	}

	void DebugUI::SetFocused( bool focused )
	{
		m_focused = focused;
	}

	void DebugUI::SetMousePosition( glm::vec2 pos )
	{
		if( m_focused )
			ImGui::GetIO().MousePos = ImVec2( pos.x, pos.y );   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
		else 
			ImGui::GetIO().MousePos = ImVec2( -1, -1 );
	}

	void DebugUI::UpdateDisplaySize()
	{
		int w, h;
		int display_w, display_h;
		glfwGetWindowSize( g_Window, &w, &h );
		glfwGetFramebufferSize( g_Window, &display_w, &display_h );
		ImGui::GetIO().DisplaySize = ImVec2( (float) w, (float) h );
		ImGui::GetIO().DisplayFramebufferScale = ImVec2( w > 0 ? ((float) display_w / w) : 0, h > 0 ? ((float) display_h / h) : 0 );
	}

	void DebugUI::StartFrame( float delta_t )
	{
		DD_PROFILE_START( DebugUI_Update );

		ImGuiIO& io = ImGui::GetIO();

		if( !m_input->IsMouseCaptured() )
		{
			SetMousePosition( m_input->GetMousePosition().Absolute );
		}

		SetFocused( m_window->IsFocused() );
		UpdateDisplaySize();

		// Setup time step
		io.DeltaTime = g_Time > 0.0 ? (float) delta_t : (float) (1.0f / 60.0f);
		g_Time += delta_t;

		for( int i = 0; i < 3; i++ )
		{
			io.MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton( g_Window, i ) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
			g_MouseJustPressed[i] = false;
		}

		io.MouseWheel = g_MouseWheel;
		g_MouseWheel = 0.0f;

		// Start the frame
		ImGui::NewFrame();

		DD_PROFILE_END();

		m_midFrame = true;
	}

	void DebugUI::EndFrame()
	{
		ImGui::Render();

		m_midFrame = false;
	}

	void DebugUI::RegisterDebugPanel( IDebugPanel& debug_panel )
	{
		m_debugPanels.push_back( &debug_panel );

		std::sort( m_debugPanels.begin(), m_debugPanels.end(),
			[]( const IDebugPanel* a, const IDebugPanel* b )
			{
				return strcmp( a->GetDebugTitle(), b->GetDebugTitle() ) < 0;
			}
		);
	}

	void DebugUI::RenderDebugPanels( ddc::World& world )
	{
		if( !m_draw )
			return;

		if( ImGui::BeginMainMenuBar() )
		{
			if( ImGui::BeginMenu( "File" ) )
			{
				if( ImGui::MenuItem( "Exit" ) )
				{
					m_window->SetToClose();
				}

				ImGui::EndMenu();
			}

			if( ImGui::BeginMenu( "Views" ) )
			{
				for( IDebugPanel* debug_view : m_debugPanels )
				{
					debug_view->AddToMenu();
				}

				ImGui::EndMenu();
			}

			if( ImGui::BeginMenu( "Profiler" ) )
			{
				bool draw = dd::Profiler::ShouldDraw();
				if( ImGui::MenuItem( "Show", nullptr, &draw ) )
				{
					dd::Profiler::EnableDraw( draw );
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if( dd::Profiler::ShouldDraw() )
		{
			dd::Profiler::Draw();
		}

		for( IDebugPanel* panel : m_debugPanels )
		{
			if( panel->IsDebugPanelOpen() )
			{
				m_midWindow = true;

				panel->DrawDebugPanel( world );

				m_midWindow = false;
			}
		}
	}
}
