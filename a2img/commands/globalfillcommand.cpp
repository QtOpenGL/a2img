/*
	a2img - a free and open source image editor
	Copyright (C) 2018 Thomas Evans

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "globalfillcommand.h"

#include "application.h"
#include "util/rectangle.h"
#include "glbuffer.h"

a2img::GlobalFillCommand::GlobalFillCommand(Layer* layer, QColor colour, int tolerance)
	: FillCommand(layer, colour, tolerance)
{
}

a2img::Texture a2img::GlobalFillCommand::renderToTexture(GlFunctions* gl, const Texture& in, bool preview)
{
	QOpenGLFramebufferObject fbo(document()->size());
	fbo.bind();

	gl->glViewport(0, 0, document()->size().width(), document()->size().height());
	gl->glClearColor(0, 0, 0, 1);
	gl->glClear(GL_COLOR_BUFFER_BIT);

	auto fillShader = app()->shaderManager.get("global_fill");
	fillShader->bind();

	// Sampler

	GLint samplerLocation = fillShader->uniformLocation("sampler");
	gl->glUniform1i(samplerLocation, 0);

	in.bind(gl, 0, TextureQuality::pixelated);

	// Resolution

	GLint resolutionUniform = fillShader->uniformLocation("resolution");
	gl->glUniform2f(resolutionUniform, document()->size().width(), document()->size().height());

	// Starting position

	GLint startingPositionUniform = fillShader->uniformLocation("startingPosition");
	gl->glUniform2f(startingPositionUniform, startingPoint().x(), startingPoint().y());

	// Tolerance

	GLint toleranceUniform = fillShader->uniformLocation("tolerance");
	gl->glUniform1f(toleranceUniform, tolerance() / 255.0F);

	// Draw

	DefaultVertexBuffer vertexBuffer(gl);
	gl->drawArrays(vertexBuffer);

	Texture mask(app(), fbo.texture(), fbo.size());

	QColor renderColour = colour();
	if(preview) {
		renderColour.setAlpha(100);
	}
	return app()->maskManager().applyMask(gl, renderColour, in, mask, BlendMode::normal);
}

