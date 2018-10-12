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

#include "movelayercommand.h"

#include "application.h"

a2img::MoveLayerCommand::MoveLayerCommand(Layer* layer, bool isMovingUp)
	: LayerCommand(layer),
	  isMovingUp_(isMovingUp)
{}

void a2img::MoveLayerCommand::redo()
{
	if(isMovingUp_) {
		moveLayer(-1);
	} else {
		moveLayer(1);
	}
}

void a2img::MoveLayerCommand::undo()
{
	if(isMovingUp_) {
		moveLayer(1);
	} else {
		moveLayer(-1);
	}
}

QString a2img::MoveLayerCommand::type() const
{
	return "moveLayer";
}

void a2img::MoveLayerCommand::serialise(DocumentWriter& storage, QJsonObject& metadata) const
{
	QJsonObject json;
	json.insert("direction", isMovingUp_ ? "up" : "down");
	metadata.insert("move", json);
}

void a2img::MoveLayerCommand::moveLayer(int offset)
{
	LayerModel* model = document()->layerModel();
	int row = model->indexFromLayer(layer()).row();
	std::unique_ptr<Layer> uniqueLayer = model->removeLayer(this, layer());
	model->addLayer(this, layer()->parent(), uniqueLayer, row + offset);

	QModelIndex newIndex = model->indexFromLayer(layer());
	app()->documentManager().setCurrentLayerIndex(newIndex);
}
