#include "Antimony.h"
#include "Timer.h"

///

color BtnStateColor(Input bt)
{
	switch (bt.getState())
	{
	case BTN_UNPRESSED:
		return color(1, 1, 1, 1);
	case BTN_PRESSED:
		return color(0, 1, 0, 1);
	case BTN_HELD:
		return color(0, 0, 1, 1);
	case BTN_RELEASED:
		return color(1, 0, 0, 1);
	case BTN_DISABLED:
		return color(0.3, 0.3, 0.3, 1);
	default:
		return color(0.3, 0.3, 0.3, 1);
	}
}

void Antimony::render_Debug()
{
	//devcon->IASetVertexBuffers(0, 1, &vertexbuffer, (UINT*)sizeof(VERTEX_BASIC), (UINT*)(0));
	setDepthBufferState(OFF);
	setShader(SHADERS_PLAIN);

	if (antimony.game.dbg_wireframe)
	{
		m_btWorld->debugDrawWorld();

		float3 p;
		float3 n;

		for (int i = 0; i < m_physEntities.size(); i++)
		{
			p = m_physEntities.at(i)->getFloat3Pos();
			n = btToFloat3(&m_physEntities.at(i)->getRigidBody()->getLinearVelocity());

			Draw3DLineThin(p, p + WORLD_SCALE * 0.1 * n, COLOR_GREEN, COLOR_GREEN, &mat_identity);

			n = btToFloat3(&m_physEntities.at(i)->getRigidBody()->getAngularVelocity());

			Draw3DLineThin(p, p + WORLD_SCALE * 0.1 * n, COLOR_RED, COLOR_RED, &mat_identity);

			float3 sp = WorldToScreen(p, &(mat_view * mat_proj), float2(window_main.width, window_main.height));
			if (sp.z > 0)
				Draw2DDot(float2(sp.x, sp.y), 10, color(1,0,0,1));
		}

		auto &mfp = m_objectsCollisionPoints[m_player.getColl()->getRigidBody()];
		if (!mfp.empty())
		{
			for (int i = 0; i < mfp.size(); i++)
			{
				p = btToFloat3(&mfp.at(i)->getPositionWorldOnB());
				n = btToFloat3(&mfp.at(i)->m_normalWorldOnB);

				Draw3DLineThin(p, p + WORLD_SCALE * 0.1 * n, COLOR_BLACK, COLOR_RED, &mat_identity);
			}
		}
		/*auto &mf = objectsCollisions[player.getColl()->rb];
		if (!mf.empty())
		{
			for (int i = 0; i < mf.size(); i++)
			{
				auto b0 = (btRigidBody*)mf.at(i)->getBody0();
				auto b1 = (btRigidBody*)mf.at(i)->getBody1();

				Vector3 p = btToFloat3(&b1->getWorldTransform().getOrigin());
				Vector3 n = btToFloat3(&b1->getTotalTorque());

				Draw3DLineThin(p, p + WORLD_SCALE * 0.1 * n, COLOR_RED, COLOR_RED, &mat_identity);
			}
		}*/
	}

	if (antimony.game.dbg_info)
	{
		switch (antimony.game.dbg_infochart)
		{
			case DBGCHART_FPS:
			{
				//devcon->IASetVertexBuffers(0, 1, &vertexbuffer, (UINT*)sizeof(VERTEX_BASIC), (UINT*)(0));
				setShader(SHADERS_PLAIN);

				render_DebugFPS(float2(antimony.display.right, antimony.display.bottom));

				break;
			}
			case DBGCHART_PIE:
			{
				break;
			}
			case DBGCHART_INPUT:
			{
				//devcon->IASetVertexBuffers(0, 1, &vertexbuffer, (UINT*)sizeof(VERTEX_BASIC), (UINT*)(0));
				setShader(SHADERS_PLAIN);

				render_DebugKeyboard(float2(0, -300));
				render_DebugMouse(float2(0, -300) + float2(90, -30));
				render_DebugController(float2(0, -300), CONTROLLER_1);

				swprintf(m_globalStr64, L"%.3f", m_mouse.X.getPos());
				Consolas.render(m_globalStr64, 10, antimony.display.right + 260, antimony.display.bottom - 180, 0xffffffff, 0);
				swprintf(m_globalStr64, L"%.3f", m_mouse.Y.getPos());
				Consolas.render(m_globalStr64, 10, antimony.display.right + 320, antimony.display.bottom - 150, 0xffffffff, 0);

				break;
			}
			default:
				break;
		}
	}
}
void Antimony::render_DebugKeyboard(float2 pos)
{
	VERTEX_BASIC vertices[] =
	{
		{ -.5, 0, 0, COLOR_WHITE },
		{ .5, 0, 0, COLOR_WHITE },
		{ .5, 1, 0, COLOR_WHITE },
		{ -.5, 1, 0, COLOR_WHITE },
		// 4
		{ -1.44, -1, 0, COLOR_WHITE },
		{ -1.75, 1, 0, COLOR_WHITE },
		{ -.8, 1, 0, COLOR_WHITE },
		{ -.56, .25, 0, COLOR_WHITE },
		{ .56, .25, 0, COLOR_WHITE },
		{ .8, 1, 0, COLOR_WHITE },
		{ 1.75, 1, 0, COLOR_WHITE },
		{ 1.44, -1, 0, COLOR_WHITE }
		// 12
	};
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));

	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2,
		// 6
		4, 5, 6,
		4, 6, 7,
		4, 7, 8,
		4, 8, 11,
		11, 8, 9,
		11, 9, 10
		// 24
	};
	FillBuffer(dev, devcon, &indexbuffer, &indices, sizeof(indices));

	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mat_world = MScaling(2200, 2200, 1) * MTranslation(pos.x, -pos.y, 0);

	// Esc
	mat_temp = MScaling(0.01, 0.009, 1) * MTranslation(0.107775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F1
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.124775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F2
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.137775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F3
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.150775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F4
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.163775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F5
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.179775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F6
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.192775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F7
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.205775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F8
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.217775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F9
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.234775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F10
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.247775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F11
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.260775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F12
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.273775, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Ins
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.28875, -0.078, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Start
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.30275, -0.078, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Pgup
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.31675, -0.078, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Canc
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.28875, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// End
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.30275, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Pgdn
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.31675, -0.090, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Backslash
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.107775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 1
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.122775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 2
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.137775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 3
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.152775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 4
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.167775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 5
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.182775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 6
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.197775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 7
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.212775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 8
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.227775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 9
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.242775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 0
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.257775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// '
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.272775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// ì
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.287775, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Backspace
	mat_temp = MScaling(0.026, 0.01, 1) * MTranslation(0.3105, -0.105, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Tab
	mat_temp = MScaling(0.0195, 0.01, 1) * MTranslation(0.11275, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Q
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1325, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// W
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1475, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_keys.forward));
	devcon->DrawIndexed(6, 0, 0);
	// E
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1625, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_keys.action));
	devcon->DrawIndexed(6, 0, 0);
	// R
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1775, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// T
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1925, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Y
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2075, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// U
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2225, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// I
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2375, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// O
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2525, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// P
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2675, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// è
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2825, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// +
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2975, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Enter
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.3125, -0.12, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// CAPS
	mat_temp = MScaling(0.022, 0.01, 1) * MTranslation(0.114, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// A
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.135, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_keys.left));
	devcon->DrawIndexed(6, 0, 0);
	// S
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.15, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_keys.backward));
	devcon->DrawIndexed(6, 0, 0);
	// D
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.165, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_keys.right));
	devcon->DrawIndexed(6, 0, 0);
	// F
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.18, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// G
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.195, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// H
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.21, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// J
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.225, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// K
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.24, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// L
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.255, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// ò
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.27, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// à
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.285, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// ù
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.3, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Enter
	mat_temp = MScaling(0.014, 0.025, 1) * MTranslation(0.317, -0.135, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Left Shift
	mat_temp = MScaling(0.014, 0.01, 1) * MTranslation(0.11, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_keys.sprint));
	devcon->DrawIndexed(6, 0, 0);
	// < >
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1275, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Z
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1425, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// X
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1575, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// C
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1725, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// V
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1875, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// B
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2025, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// N
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2175, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// M
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2325, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// ,
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2475, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// .
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2625, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// -
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2775, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Right Shift
	mat_temp = MScaling(0.036, 0.01, 1) * MTranslation(0.3055, -0.15, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Left Ctrl
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.10775, -0.165, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Fn
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.12275, -0.165, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Start
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.13775, -0.165, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Left Alt
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.15275, -0.165, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Spacebar
	mat_temp = MScaling(0.07, 0.01, 1) * MTranslation(0.19775, -0.165, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_keys.jump));
	devcon->DrawIndexed(6, 0, 0);
	// Alt Gr
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.24275, -0.165, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Win
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.25775, -0.165, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Right Ctrl
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.27275, -0.165, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Up
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.30275, -0.163, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Left
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.28875, -0.175, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Down
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.30275, -0.175, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Right
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.31675, -0.175, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
}
void Antimony::render_DebugMouse(float2 pos)
{
	VERTEX_BASIC vertices[] =
	{
		{ -.5, 0, 0, COLOR_WHITE },
		{ .5, 0, 0, COLOR_WHITE },
		{ .5, 1, 0, COLOR_WHITE },
		{ -.5, 1, 0, COLOR_WHITE },
		// 4
		{ -1.44, -1, 0, COLOR_WHITE },
		{ -1.75, 1, 0, COLOR_WHITE },
		{ -.8, 1, 0, COLOR_WHITE },
		{ -.56, .25, 0, COLOR_WHITE },
		{ .56, .25, 0, COLOR_WHITE },
		{ .8, 1, 0, COLOR_WHITE },
		{ 1.75, 1, 0, COLOR_WHITE },
		{ 1.44, -1, 0, COLOR_WHITE }
		// 12
	};
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));

	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2,
		// 6
		4, 5, 6,
		4, 6, 7,
		4, 7, 8,
		4, 8, 11,
		11, 8, 9,
		11, 9, 10
		// 24
	};
	FillBuffer(dev, devcon, &indexbuffer, &indices, sizeof(indices));

	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mat_world = MScaling(2200, 2200, 1) * MTranslation(pos.x, -pos.y, 0);

	// Left
	mat_temp = MScaling(0.0075, 0.01, 1) * MTranslation(0.1, -0.050, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_mouse.LMB));
	devcon->DrawIndexed(6, 0, 0);

	// Middle
	mat_temp = MScaling(0.0035, 0.01, 1) * MTranslation(0.11, -0.050, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_mouse.MMB));
	devcon->DrawIndexed(6, 0, 0);

	// Right
	mat_temp = MScaling(0.0075, 0.01, 1) * MTranslation(0.12, -0.050, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_mouse.RMB));
	devcon->DrawIndexed(6, 0, 0);

	// Mouse
	mat_temp = MScaling(0.028, 0.0235, 1) * MTranslation(0.11, -0.0775, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
}
void Antimony::render_DebugController(float2 pos, unsigned char c)
{
	VERTEX_BASIC vertices[] =
	{
		{ -.5, 0, 0, COLOR_WHITE },
		{ .5, 0, 0, COLOR_WHITE },
		{ .5, 1, 0, COLOR_WHITE },
		{ -.5, 1, 0, COLOR_WHITE },
		// 4
		{ -1.44, -1, 0, COLOR_WHITE },
		{ -1.75, 1, 0, COLOR_WHITE },
		{ -.8, 1, 0, COLOR_WHITE },
		{ -.56, .25, 0, COLOR_WHITE },
		{ .56, .25, 0, COLOR_WHITE },
		{ .8, 1, 0, COLOR_WHITE },
		{ 1.75, 1, 0, COLOR_WHITE },
		{ 1.44, -1, 0, COLOR_WHITE }
		// 12
	};
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));

	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2,
		// 6
		4, 5, 6,
		4, 6, 7,
		4, 7, 8,
		4, 8, 11,
		11, 8, 9,
		11, 9, 10
		// 24
	};
	FillBuffer(dev, devcon, &indexbuffer, &indices, sizeof(indices));

	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mat_world = MScaling(2200, 2200, 1) * MTranslation(pos.x, -pos.y, 0);

	// Layout
	mat_temp = MRotZ(MATH_PI) * MScaling(0.0175, 0.0175, 1) * MTranslation(0.23, -0.050, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(18, 6, 0);

	// Back
	mat_temp = MScaling(0.0045, 0.004, 1) * MTranslation(0.225, -0.045, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].Back));
	devcon->DrawIndexed(6, 0, 0);

	// Start
	mat_temp = MScaling(0.0045, 0.004, 1) * MTranslation(0.235, -0.045, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].Start));
	devcon->DrawIndexed(6, 0, 0);

	// Left
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2075, -0.045, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].Left));
	devcon->DrawIndexed(6, 0, 0);

	// Right
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2175, -0.045, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].Right));
	devcon->DrawIndexed(6, 0, 0);

	// Down
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2125, -0.05, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].Down));
	devcon->DrawIndexed(6, 0, 0);

	// Up
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2125, -0.04, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].Up));
	devcon->DrawIndexed(6, 0, 0);

	// X
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2425, -0.045, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].X));
	devcon->DrawIndexed(6, 0, 0);

	// B
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2525, -0.045, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].B));
	devcon->DrawIndexed(6, 0, 0);

	// A
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2475, -0.05, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].A));
	devcon->DrawIndexed(6, 0, 0);

	// Y
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2475, -0.04, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].Y));
	devcon->DrawIndexed(6, 0, 0);

	// LT
	mat_temp = MScaling(0.0075, 0.0075, 1) * MTranslation(0.2125, -0.0325, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].LT));
	devcon->DrawIndexed(6, 0, 0);

	// RT
	mat_temp = MScaling(0.0075, 0.0075, 1) * MTranslation(0.2475, -0.0325, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].RT));
	devcon->DrawIndexed(6, 0, 0);

	// LB
	mat_temp = MScaling(0.01, 0.0025, 1) * MTranslation(0.2125, -0.0325, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].LB));
	devcon->DrawIndexed(6, 0, 0);

	// RB
	mat_temp = MScaling(0.01, 0.0025, 1) * MTranslation(0.2475, -0.0325, 0);
	setView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(m_controller[c].RB));
	devcon->DrawIndexed(6, 0, 0);

	// Left Circle
	Draw2DEllipses(15.4, 15.4, pos.x + 487.3, pos.y + 116.6, color(.4, .4, .4, 1));

	// Right Circle
	Draw2DEllipses(15.4, 15.4, pos.x + 524.7, pos.y + 116.6, color(.4, .4, .4, 1));

	// Left Stick
	Draw2DEllipses(9.9, 9.9, pos.x + 487.3 + m_controller[c].LX.getVel() * 4.4, pos.y + 116.6 + m_controller[c].LY.getVel() * 4.4, BtnStateColor(m_controller[c].LS));

	// Right Stick
	Draw2DEllipses(9.9, 9.9, pos.x + 524.7 + m_controller[c].RX.getVel() * 4.4, pos.y + 116.6 + m_controller[c].RY.getVel() * 4.4, BtnStateColor(m_controller[c].RS));

}
void Antimony::render_DebugFPS(float2 pos)
{
	short cpu = m_cpuUsage.GetUsage(0);

	K32GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&m_pmc, sizeof(m_pmc));
	m_physMemUsedByMe = m_pmc.WorkingSetSize;
	double physMemPercent = double(m_physMemUsedByMe) / double(m_totalPhysMem) * 100;
	double physMemMB = double(m_physMemUsedByMe) / 1048576;
	double totalPhysMemMB = double(m_totalPhysMem) / 1048576;
	double totalPhysMemGB = double(m_totalPhysMem) / 1073741824;

	color fps_bar;
	color transp = color(0, 0, 0, 0.2);
	color border = color(0, 0.73, 0.73, 1);
	color border2 = color(1, 0.06, 0.6, 1);
	int fps_line = 0;
	if (timer.GetFramesCount() < 256)
	{
		fps_bar = color((float)(255 - timer.GetFramesCount()) / (float)255, (float)(timer.GetFramesCount()) / (float)255, 0, 1);
		fps_line = 0.7843 * timer.GetFramesCount() + pos.x - 260;
	}
	else
	{
		fps_bar = color(0, (float)(510 - timer.GetFramesCount()) / (float)255, (float)(timer.GetFramesCount() - 255) / (float)255, 1);
		fps_line = 0.7843 * 256 + pos.x - 260;
	}
	Draw2DRectangle(200, 10, pos.x - 260, pos.y - 174, color(0, 0, 0, 0.2)); // FPS bg
	Draw2DLineThick(float2(pos.x - 260, pos.y - 179), float2(fps_line, pos.y - 179), 10, fps_bar, fps_bar); // FPS bar
	Draw2DRectBorderThick(200, 10, pos.x - 260, pos.y - 174, 1, color(0, 0.73, 0.73, 1)); // FPS edge
	Draw2DLineThick(float2(pos.x - 252.157, pos.y - 174), float2(pos.x - 252.157, pos.y - 184), 1, border, border); // 10 FPS
	Draw2DLineThick(float2(pos.x - 236.471, pos.y - 174), float2(pos.x - 236.471, pos.y - 184), 1, border, border); // 30 FPS
	Draw2DLineThick(float2(pos.x - 212.942, pos.y - 174), float2(pos.x - 212.942, pos.y - 184), 1, border, border); // 60 FPS
	Draw2DLineThick(float2(pos.x - 181.57, pos.y - 174), float2(pos.x - 181.57, pos.y - 184), 1, border, border); // 100 FPS
	Draw2DLineThick(float2(pos.x - 103.14, pos.y - 174), float2(pos.x - 103.14, pos.y - 184), 1, border, border); // 200 FPS

	Draw2DRectangle(200, 54, pos.x - 260, pos.y - 108, transp);
	for (unsigned int i = 1; i < m_cpuUsage.usageStream.size(); i++)
	{
		Draw2DLineThin(
			float2(
				pos.x - 260 + 2 * (i - 1) * ((float)100 / ((float)m_cpuUsage.GetMaxRecords() - 1)),
				pos.y - 110 - (float)m_cpuUsage.usageStream.at(i - 1) * 0.5),
			float2(
				pos.x - 260 + 2 * (i) * ((float)100 / ((float)m_cpuUsage.GetMaxRecords() - 1)),
				pos.y - 110 - (float)m_cpuUsage.usageStream.at(i) * 0.5),
			color(1, 0.06, 0.6, 1), color(1, 0.06, 0.6, 1));
	}
	Draw2DRectBorderThick(200, 54, pos.x - 260, pos.y - 108, 1, border);

	Draw2DRectangle(200, 10, pos.x - 260, pos.y - 83, transp);
	Draw2DLineThick(float2(pos.x - 260, pos.y - 88), float2(1.945 * cpu + pos.x - 258, pos.y - 88), 10, border2, border2);
	Draw2DRectBorderThick(200, 10, pos.x - 260, pos.y - 83, 1, border);

	swprintf(m_globalStr64, L"Total entities: %i", m_physEntities.size());
	Consolas.render(m_globalStr64, 10, antimony.display.width - 260, antimony.display.height - 214, 0xffffffff, 0);

	swprintf(m_globalStr64, L"FPS: %.2f (%i)", timer.GetFPSStamp(), timer.GetFramesCount());
	Consolas.render(m_globalStr64, 10, antimony.display.width - 260, antimony.display.height - 200, 0xffffffff, 0);
	swprintf(m_globalStr64, L"Max: %.2f", timer.maxFps);
	Consolas.render(m_globalStr64, 10, antimony.display.width - 125, antimony.display.height - 200, 0xffffffff, 0);

	swprintf(m_globalStr64, L"CPU usage: %d%%", cpu);
	Consolas.render(m_globalStr64, 10, antimony.display.width - 260, antimony.display.height - 78, 0xffffffff, 0);
	swprintf(m_globalStr64, L"RAM usage: %.2f%%", physMemPercent);
	Consolas.render(m_globalStr64, 10, antimony.display.width - 260, antimony.display.height - 64, 0xffffffff, 0);
	swprintf(m_globalStr64, L"Alloc.: %.3f MB / %.3f GB", physMemMB, totalPhysMemGB);
	Consolas.render(m_globalStr64, 10, antimony.display.width - 260, antimony.display.height - 50, 0xffffffff, 0);
}