// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"	
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "ImageDifferencer.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class AI_API AImageDifferencer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AImageDifferencer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Gets all the files in a given directory.
	TArray<FString> AImageDifferencer::GetAllFilesInDirectory(const FString directory, const bool fullPath, const FString onlyFilesStartingWith, const FString onlyFilesWithExtension);

	// Calculates the changed pixel nearest the center of screen
	UFUNCTION(BlueprintCallable, Category = "Utilities")
		FVector2D getChangedPixel(FString imagePath1, FString imagePath2);
	
};
