// Fill out your copyright notice in the Description page of Project Settings.

#include "AI.h"
#include "ImageDifferencer.h"
#include "Kismet/KismetStringLibrary.h"
#include "Engine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

using namespace cv;


// Sets default values
AImageDifferencer::AImageDifferencer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AImageDifferencer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AImageDifferencer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

/**
Gets all the files in a given directory.
@param directory The full path of the directory we want to iterate over.
@param fullpath Whether the returned list should be the full file paths or just the filenames.
@param onlyFilesStartingWith Will only return filenames starting with this string. Also applies onlyFilesEndingWith if specified.
@param onlyFilesEndingWith Will only return filenames ending with this string (it looks at the extension as well!). Also applies onlyFilesStartingWith if specified.
@return A list of files (including the extension).
*/
TArray<FString> AImageDifferencer::GetAllFilesInDirectory(const FString directory, const bool fullPath, const FString onlyFilesStartingWith, const FString onlyFilesWithExtension)
{
	// Get all files in directory
	TArray<FString> directoriesToSkip;
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FLocalTimestampDirectoryVisitor Visitor(PlatformFile, directoriesToSkip, directoriesToSkip, false);
	PlatformFile.IterateDirectory(*directory, Visitor);
	TArray<FString> files;

	for (TMap<FString, FDateTime>::TIterator TimestampIt(Visitor.FileTimes); TimestampIt; ++TimestampIt)
	{
		const FString filePath = TimestampIt.Key();
		const FString fileName = FPaths::GetCleanFilename(filePath);
		bool shouldAddFile = true;

		// Check if filename starts with required characters
		if (!onlyFilesStartingWith.IsEmpty())
		{
			const FString left = fileName.Left(onlyFilesStartingWith.Len());

			if (!(fileName.Left(onlyFilesStartingWith.Len()).Equals(onlyFilesStartingWith)))
				shouldAddFile = false;
		}

		// Check if file extension is required characters
		if (!onlyFilesWithExtension.IsEmpty())
		{
			if (!(FPaths::GetExtension(fileName, false).Equals(onlyFilesWithExtension, ESearchCase::IgnoreCase)))
				shouldAddFile = false;
		}

		// Add full path to results
		if (shouldAddFile)
		{
			files.Add(fullPath ? filePath : fileName);
		}
	}

	return files;
}

FVector2D AImageDifferencer::getChangedPixel(FString imagePath1, FString imagePath2)
{
	FVector2D changedPixelScreenProportions = FVector2D(-1, -1);
	FString path = FPaths::GameDir();

	FString screenshotsPath = path + "Saved/Screenshots/Windows/";
	TArray<FString> filesInDirectory = GetAllFilesInDirectory(screenshotsPath, true, "", "png");
	
	if (filesInDirectory.Num() > 1)
	{
		cv::Mat mat1 = cv::imread(TCHAR_TO_UTF8(*filesInDirectory[0]));
		cv::Mat mat2 = cv::imread(TCHAR_TO_UTF8(*filesInDirectory[1]));

		cv::Size s = mat1.size();
		int rows = s.height;
		int cols = s.width;

		float bestDistance = 999;
		int changedPixels = 0;

		for (int i = 0; i < cols; i++)
		{
			for (int j = 0; j < rows; j++)
			{
				cv::Vec3b pixel1 = mat1.at<cv::Vec3b>(j, i);
				cv::Vec3b pixel2 = mat2.at<cv::Vec3b>(j, i);
				if (abs(pixel1[0] - pixel2[0]) > 50)
				{
					/*
					GEngine->AddOnScreenDebugMessage(-1, INFINITE, FColor::Red,
						UKismetStringLibrary::Conv_IntToString(pixel1[0]));
					GEngine->AddOnScreenDebugMessage(-1, INFINITE, FColor::Red,
						UKismetStringLibrary::Conv_IntToString(pixel2[0]));
						*/
						

					changedPixels++;
					float distance = sqrt(pow(cols/2 - i, 2) + pow(rows/2 - j, 2));

					if (distance < bestDistance)
					{
						bestDistance = distance;
						changedPixelScreenProportions = FVector2D((float)i / cols, (float)j / rows);
					}
				}
			}
		}

		/*
		GEngine->AddOnScreenDebugMessage(-1, INFINITE, FColor::Red,
			UKismetStringLibrary::Conv_FloatToString(changedPixels));
			*/


		
		for (int i = 0; i < filesInDirectory.Num(); i++) 
		{
			bool a = FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*filesInDirectory[i]);
		}
		
	}
	return changedPixelScreenProportions;
}