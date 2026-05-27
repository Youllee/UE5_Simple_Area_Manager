#pragma once

#include "ComponentVisualizer.h"

class AAreaManagerAreaActor;
class AVolume;
class USelection;

class FAreaManagerVisualizer : public FComponentVisualizer
{
public:
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;

private:
	void CollectSelectedAndConnectedAreas(USelection* SelectedActors, TArray<AAreaManagerAreaActor*>& OutAreas) const;
	void DrawSelectedArea(AAreaManagerAreaActor* Area, const FSceneView* View, FPrimitiveDrawInterface* PDI) const;
	void DrawAreaLabel(AAreaManagerAreaActor* Area, const FSceneView* View, FCanvas* Canvas, const FLinearColor& LabelColor) const;
	void DrawAreaVolumes(AAreaManagerAreaActor* Area, FPrimitiveDrawInterface* PDI) const;
	void DrawVolumeBox(AVolume* Volume, FPrimitiveDrawInterface* PDI) const;

private:
	const FColor SelectedPointColor = FColor::Cyan;
	const FColor SelectedLineColor = FColor::Blue;
	const FColor ConnectedPointColor = FColor::Yellow;
	const FLinearColor SelectedLabelColor = FLinearColor(0.35f, 1.0f, 1.0f, 1.0f);
	const FLinearColor ConnectedLabelColor = FLinearColor(1.0f, 0.95f, 0.25f, 1.0f);
	const FLinearColor LabelBackgroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.68f);
	const FLinearColor VolumeColor = FLinearColor(0.0f, 0.65f, 1.0f, 0.18f);

	const float PointSize = 25.0f;
	const float LineThickness = 8.0f;
	const double LabelVerticalOffset = 28.0;
	const FVector2D LabelPadding = FVector2D(6.0, 3.0);
};
