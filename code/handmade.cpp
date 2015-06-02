#include "handmade.h"

internal void
GameOutputSound(game_sound_output_buffer *SoundBuffer, int ToneHz)
{
    local_persist real32 tSine;
    int16 ToneVolume = 6000;
    int WavePeriod = SoundBuffer->SamplesPerSecond/ToneHz;

    int16 *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0;
        SampleIndex < SoundBuffer->SampleCount;
        ++SampleIndex)
    {
        /// TODO(Dennis): Draw this out for people
        real32 SineValue = sinf(tSine);
        int16 SampleValue = (int16)(SineValue * ToneVolume);
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;

        tSine += 2.0f * Pi32 * 1.0f / (real32)WavePeriod;
    }
}

internal void
RenderWeirdGradient(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset)
{

    uint8 *Row = (uint8 *)Buffer->Memory;

	for(int Y=0;
        Y < Buffer->Height;
        Y++)
	{
		uint32 *Pixel = (uint32 *) Row;
		for(int X=0;
		    X < Buffer->Width;
		    X++)
		{
			/*
			Little Endian
			Pixel in Memory: BB GG RR xx
			0x xxRRGGBB
			*/
			uint8 Blue = (uint8)(X + BlueOffset);
			uint8 Green = (uint8)(Y + GreenOffset);

			*Pixel++ = ((Green << 8) | Blue);
		}

		Row = Row + Buffer->Pitch;
	}
}

internal void
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer,
                    game_sound_output_buffer *SoundBuffer)
{
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) == (ArrayCount(Input->Controllers[0].Buttons)));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if(!Memory->IsInitialized)
    {
#if HANDMADE_INTERNAL
        char *Filename = __FILE__;

        debug_read_file_result File = DEBUGPlatformReadEntireFile(Filename);
        if(File.Contents)
        {
            DEBUGPlatformWriteEntireFile("test.out", File.ContentsSize, File.Contents);
            DEBUGPlatformFreeFileMemory(File.Contents);
        }
#endif // HANDMADE_INTERNAL

        GameState->ToneHz = 512;

        /// TODO(Dennis): This may be more appropriate to do in the platform layer
        Memory->IsInitialized = true;
    }

    for(int ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ++ControllerIndex)
    {
        game_controller_input *Controller = GetController(Input, ControllerIndex);
        if(Controller->IsAnalog)
        {
            /// NOTE(Dennis): Use analog movement tuning
            GameState->BlueOffset += (int)(4.0f*Controller->StickAverageX);
            GameState->ToneHz = 512 + (int)(128.0f*Controller->StickAverageY);
        }
        else
        {
            /// NOTE(Dennis): Use digital movement tuning
            if(Controller->MoveLeft.EndedDown)
            {
                GameState->BlueOffset -= 1;
            }

            if(Controller->MoveRight.EndedDown)
            {
                GameState->BlueOffset += 1;
            }
        }

        /// Input.AButtonEndedDown;
        /// Input.AButtonHalfTransitionCount;
        if(Controller->ActionDown.EndedDown)
        {
            GameState->GreenOffset += 1;
        }
    }
    /// TODO(Dennis): Allow sample offsets here for more robust platform options
    GameOutputSound(SoundBuffer, GameState->ToneHz);
    RenderWeirdGradient(Buffer, GameState->BlueOffset, GameState->GreenOffset);
}
