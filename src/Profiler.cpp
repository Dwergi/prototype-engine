//
// Profiler.cpp
// Copyright (C) Sebastian Nordgren 
// January 27th 2019
//

#include "PCH.h"
#include "Profiler.h"

#include "fmt/format.h"

namespace dd
{
	ProfilerValue::ProfilerValue( const char* name, float initial ) :
		m_name( name )
	{
		for( float& f : m_values )
		{
			f = initial;
		}

		m_sliding = initial;
	}

	void ProfilerValue::Increment()
	{
		++m_values[m_index];
	}

	void ProfilerValue::SetValue( float value )
	{
		m_values[m_index] = value;
	}

	float ProfilerValue::GetValue( int index ) const
	{
		DD_ASSERT( index >= 0 && index < VALUE_COUNT );

		return m_values[index];
	}

	void ProfilerValue::BeginFrame()
	{
		++m_index;

		if( m_index == VALUE_COUNT )
		{
			m_index = 0;
		}

		m_values[m_index] = 0;
	}

	void ProfilerValue::EndFrame()
	{
		float total = 0;
		for( float i : m_values )
		{
			total += i;
		}

		m_sliding = total / VALUE_COUNT;
	}

	float ProfilerValueGetter( void* data, int index )
	{
		ProfilerValue* value = (ProfilerValue*) data;

		int actual = value->Index() + index + 1;
		if( actual >= ProfilerValue::VALUE_COUNT )
		{
			actual -= ProfilerValue::VALUE_COUNT;
		}

		float f = value->GetValue( actual );
		return f;
	}

	void ProfilerValue::Draw()
	{
		if( ImGui::TreeNodeEx( this, ImGuiTreeNodeFlags_CollapsingHeader, "%s: %.2f", m_name.c_str(), m_sliding ) )
		{
			ImGui::PlotLines( "", &ProfilerValueGetter, this, VALUE_COUNT - 1, 0, nullptr, 0, 50, ImVec2( 200, 50 ) );

			ImGui::TreePop();
		}
	}

	static ProfilerValue g_materialChanged( "Material Changes" );
	static ProfilerValue g_shaderChanged( "Shader Changes" );
	static ProfilerValue g_renderStateChanged( "Render State Changes" );
	static ProfilerValue g_meshesRendered( "Meshes Rendered" );
	static bool g_draw = false;

	void Profiler::MaterialChanged()
	{
		g_materialChanged.Increment();
	}

	void Profiler::ShaderChanged()
	{
		g_shaderChanged.Increment();
	}

	void Profiler::RenderStateChanged()
	{
		g_renderStateChanged.Increment();
	}

	void Profiler::MeshRendered()
	{
		g_meshesRendered.Increment();
	}

	void Profiler::BeginFrame()
	{
		g_materialChanged.BeginFrame();
		g_shaderChanged.BeginFrame();
		g_renderStateChanged.BeginFrame();
		g_meshesRendered.BeginFrame();
	}

	void Profiler::EndFrame()
	{
		g_materialChanged.EndFrame();
		g_shaderChanged.EndFrame();
		g_renderStateChanged.EndFrame();
		g_meshesRendered.EndFrame();
	}

	void Profiler::Draw()
	{
		g_materialChanged.Draw();
		g_shaderChanged.Draw();
		g_renderStateChanged.Draw();
		g_meshesRendered.Draw();
	}

	void Profiler::EnableDraw( bool draw )
	{
		g_draw = draw;
	}

	bool Profiler::ShouldDraw()
	{
		return g_draw;
	}
}

