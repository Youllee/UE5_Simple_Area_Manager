#include "AreaManagerVisualizer.h"
#include "AreaManagerAreaActor.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Editor.h"
#include "EditorModes.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "DynamicMeshBuilder.h"
#include "GameFramework/Volume.h"
#include "Materials/Material.h"
#include "PrimitiveDrawInterface.h"
#include "SceneManagement.h"
#include "SceneView.h"
#include "Selection.h"

void FAreaManagerVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (GEditor == nullptr || Component == nullptr || View == nullptr || PDI == nullptr)
	{
		return;
	}

	USelection* SelectedActors = GEditor->GetSelectedActors();
	if (SelectedActors == nullptr || SelectedActors->Num() <= 0)
	{
		return;
	}

	AAreaManagerAreaActor* SelectedArea = Cast<AAreaManagerAreaActor>(Component->GetOwner());
	if (IsValid(SelectedArea) == false)
	{
		return;
	}

	const AAreaManagerAreaActor* TopSelectedArea = SelectedActors->GetTop<AAreaManagerAreaActor>();
	if (TopSelectedArea == nullptr || TopSelectedArea != SelectedArea)
	{
		return;
	}

	if (IsValid(SelectedArea->GetWorld()) == false)
	{
		return;
	}

	TArray<AAreaManagerAreaActor*> SelectedAndConnectedAreas;
	CollectSelectedAndConnectedAreas(SelectedActors, SelectedAndConnectedAreas);
	for (AAreaManagerAreaActor* Area : SelectedAndConnectedAreas)
	{
		if (SelectedActors->IsSelected(Area))
		{
			DrawAreaVolumes(Area, PDI);
		}
	}

	TArray<UObject*> SelectedAreaObjects;
	SelectedActors->GetSelectedObjects(AAreaManagerAreaActor::StaticClass(), SelectedAreaObjects);
	for (UObject* SelectedObject : SelectedAreaObjects)
	{
		DrawSelectedArea(Cast<AAreaManagerAreaActor>(SelectedObject), View, PDI);
	}
}

void FAreaManagerVisualizer::DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (GEditor == nullptr || Component == nullptr || Viewport == nullptr || View == nullptr || Canvas == nullptr)
	{
		return;
	}

	USelection* SelectedActors = GEditor->GetSelectedActors();
	if (SelectedActors == nullptr || SelectedActors->Num() <= 0)
	{
		return;
	}

	AAreaManagerAreaActor* SelectedArea = Cast<AAreaManagerAreaActor>(Component->GetOwner());
	if (IsValid(SelectedArea) == false)
	{
		return;
	}

	const AAreaManagerAreaActor* TopSelectedArea = SelectedActors->GetTop<AAreaManagerAreaActor>();
	if (TopSelectedArea == nullptr || TopSelectedArea != SelectedArea)
	{
		return;
	}

	TArray<AAreaManagerAreaActor*> SelectedAndConnectedAreas;
	CollectSelectedAndConnectedAreas(SelectedActors, SelectedAndConnectedAreas);
	for (AAreaManagerAreaActor* Area : SelectedAndConnectedAreas)
	{
		const FLinearColor LabelColor = SelectedActors->IsSelected(Area) ? SelectedLabelColor : ConnectedLabelColor;
		DrawAreaLabel(Area, View, Canvas, LabelColor);
	}
}

void FAreaManagerVisualizer::CollectSelectedAndConnectedAreas(USelection* SelectedActors, TArray<AAreaManagerAreaActor*>& OutAreas) const
{
	if (SelectedActors == nullptr)
	{
		return;
	}

	TArray<UObject*> SelectedAreaObjects;
	SelectedActors->GetSelectedObjects(AAreaManagerAreaActor::StaticClass(), SelectedAreaObjects);
	for (UObject* SelectedObject : SelectedAreaObjects)
	{
		AAreaManagerAreaActor* Area = Cast<AAreaManagerAreaActor>(SelectedObject);
		if (IsValid(Area) == false)
		{
			continue;
		}

		OutAreas.AddUnique(Area);

		for (AAreaManagerAreaActor* ConnectedArea : Area->GetConnectedAreas())
		{
			if (IsValid(ConnectedArea))
			{
				OutAreas.AddUnique(ConnectedArea);
			}
		}
	}
}

void FAreaManagerVisualizer::DrawSelectedArea(AAreaManagerAreaActor* Area, const FSceneView* View, FPrimitiveDrawInterface* PDI) const
{
	if (IsValid(Area) == false)
	{
		return;
	}

	const FVector AreaLocation = Area->GetActorLocation();
	const float DistanceToCamera = FVector::Dist(View->ViewLocation, AreaLocation);
	const float DistanceScale = FMath::Clamp(2000.0f / FMath::Max(DistanceToCamera, 100.0f), 0.3f, 3.0f);
	const float AdjustedPointSize = PointSize * DistanceScale;

	PDI->DrawPoint(AreaLocation, SelectedPointColor, AdjustedPointSize, SDPG_Foreground);

	const TArray<AAreaManagerAreaActor*> ConnectedAreas = Area->GetConnectedAreas();
	for (AAreaManagerAreaActor* ConnectedArea : ConnectedAreas)
	{
		if (IsValid(ConnectedArea) == false || ConnectedArea == Area)
		{
			continue;
		}

		PDI->DrawPoint(ConnectedArea->GetActorLocation(), ConnectedPointColor, AdjustedPointSize, SDPG_Foreground);
		PDI->DrawLine(AreaLocation, ConnectedArea->GetActorLocation(), SelectedLineColor, SDPG_Foreground, LineThickness);
	}
}

void FAreaManagerVisualizer::DrawAreaLabel(AAreaManagerAreaActor* Area, const FSceneView* View, FCanvas* Canvas, const FLinearColor& LabelColor) const
{
	if (IsValid(Area) == false || View == nullptr || Canvas == nullptr || GEngine == nullptr)
	{
		return;
	}

	UFont* LabelFont = GEngine->GetMediumFont();
	if (LabelFont == nullptr)
	{
		return;
	}

	const FName AreaName = Area->GetAreaName();
	const FString Label = AreaName.IsNone() ? Area->GetActorNameOrLabel() : AreaName.ToString();
	if (Label.IsEmpty())
	{
		return;
	}

	FVector2D PixelLocation;
	if (View->WorldToPixel(Area->GetActorLocation(), PixelLocation) == false)
	{
		return;
	}

	const double InvDpiScale = 1.0 / Canvas->GetDPIScale();
	PixelLocation *= InvDpiScale;

	const double ViewWidth = View->UnscaledViewRect.Width() * InvDpiScale;
	const double ViewHeight = View->UnscaledViewRect.Height() * InvDpiScale;
	if (PixelLocation.X < 0.0 || PixelLocation.Y < 0.0 || PixelLocation.X > ViewWidth || PixelLocation.Y > ViewHeight)
	{
		return;
	}

	double LabelSizeX = 0.0;
	double LabelSizeY = 0.0;
	UCanvas::StrLen(LabelFont, FStringView(*Label, Label.Len()), LabelSizeX, LabelSizeY, false, Canvas);

	const double LabelX = PixelLocation.X - (LabelSizeX * 0.5);
	const double LabelY = PixelLocation.Y - LabelVerticalOffset - LabelSizeY;
	const FVector2D BackgroundPosition(LabelX - LabelPadding.X, LabelY - LabelPadding.Y);
	const FVector2D BackgroundSize(LabelSizeX + (LabelPadding.X * 2.0), LabelSizeY + (LabelPadding.Y * 2.0));

	FCanvasTileItem BackgroundItem(BackgroundPosition, BackgroundSize, LabelBackgroundColor);
	BackgroundItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(BackgroundItem);

	Canvas->DrawShadowedString(LabelX, LabelY, FStringView(*Label, Label.Len()), LabelFont, LabelColor);
}

void FAreaManagerVisualizer::DrawAreaVolumes(AAreaManagerAreaActor* Area, FPrimitiveDrawInterface* PDI) const
{
	if (IsValid(Area) == false)
	{
		return;
	}

	for (AVolume* Volume : Area->CollectAreaVolumes())
	{
		DrawVolumeBox(Volume, PDI);
	}
}

void FAreaManagerVisualizer::DrawVolumeBox(AVolume* Volume, FPrimitiveDrawInterface* PDI) const
{
	if (IsValid(Volume) == false || IsValid(Volume->GetRootComponent()) == false || PDI == nullptr || PDI->View == nullptr || GEngine == nullptr)
	{
		return;
	}

	const FBox Box = Volume->GetRootComponent()->CalcLocalBounds().GetBox();
	if (!Box.IsValid)
	{
		return;
	}

	FDynamicMeshBuilder MeshBuilder(PDI->View->GetFeatureLevel());

	MeshBuilder.AddVertices({
		FVector3f(Box.Min),
		FVector3f(Box.Max.X, Box.Min.Y, Box.Min.Z),
		FVector3f(Box.Max.X, Box.Max.Y, Box.Min.Z),
		FVector3f(Box.Min.X, Box.Max.Y, Box.Min.Z),
		FVector3f(Box.Min.X, Box.Min.Y, Box.Max.Z),
		FVector3f(Box.Max.X, Box.Min.Y, Box.Max.Z),
		FVector3f(Box.Max),
		FVector3f(Box.Min.X, Box.Max.Y, Box.Max.Z)
	});

	MeshBuilder.AddTriangles({
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		0, 4, 5, 0, 5, 1,
		1, 5, 6, 1, 6, 2,
		2, 6, 7, 2, 7, 3,
		3, 7, 4, 3, 4, 0
	});

	FMaterialRenderProxy* BaseProxy = (GEngine->GeomMaterial != nullptr) ? GEngine->GeomMaterial->GetRenderProxy() : nullptr;
	if (BaseProxy == nullptr)
	{
		return;
	}

	FDynamicColoredMaterialRenderProxy* ColorProxy = new FDynamicColoredMaterialRenderProxy(BaseProxy, VolumeColor);
	PDI->RegisterDynamicResource(ColorProxy);

	MeshBuilder.Draw(PDI, Volume->GetTransform().ToMatrixWithScale(), ColorProxy, SDPG_Foreground, true, false);
}
