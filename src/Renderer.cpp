#include "PrecompiledHeader.h"
#include "Renderer.h"

#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Window.h"

#include "GL/gl3w.h"

namespace dd
{
	Renderer::Renderer( Window& window )
	{

	}

	Renderer::~Renderer()
	{

	}

	ShaderProgram CreateShaders()
	{
		Vector<Shader> shaders;

		Shader vert = Shader::Create( String8( "vertex" ), String8( "" ), Shader::Type::Vertex );
		DD_ASSERT( vert.IsValid() );
		shaders.Add( vert );

		Shader pixel = Shader::Create( String8( "pixel" ), String8( "" ), Shader::Type::Pixel );
		DD_ASSERT( pixel.IsValid() );
		shaders.Add( pixel );

		ShaderProgram program = ShaderProgram::Create( String8( "default" ), shaders );
		DD_ASSERT( program.IsValid() );

		return program;
	}

	void Renderer::Init()
	{
		m_camera = new Camera( *m_window );

		// TODO: This should be in some renderer type of class.
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glFrontFace( GL_CCW );
		glCullFace( GL_BACK );

		ShaderProgram shader = CreateShaders();

		m_shaders.Add( new ShaderProgram( shader ) );
		m_meshes.Add( new Mesh( shader ) );
	}

	void Renderer::Render( float delta_t )
	{
		for( Mesh* mesh : m_meshes )
		{
			mesh->Render( *m_camera );
		}
	}

	Camera& Renderer::GetCamera() const
	{
		return *m_camera;
	}
}