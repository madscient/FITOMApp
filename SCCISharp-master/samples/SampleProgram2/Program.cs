using System;
using SCCISharp;

namespace SampleProgram2
{
    internal class Program
    {
        private static void Main()
        {
            using (var manager = SoundInterfaceManager.GetInstance())
            {
                manager.Reset();
                manager.SetAcquisitionMode(AcquisitionMode.Near);

                SoundChip chip;
                if (!manager.TryGetSoundChip(out chip, ChipType.YM2203, 8.0))
                {
                    Console.Error.WriteLine("このサンプルでは {0} が必要です。", ChipType.YM2203);
                    return;
                }

                using (chip)
                {
                    {
                        chip.SetRegister(0x00, 0x01);
                        chip.SetRegister(0x01, 0x00);
                        chip.SetRegister(0x02, 0x01);
                        chip.SetRegister(0x03, 0x00);
                        chip.SetRegister(0x04, 0x01);
                        chip.SetRegister(0x05, 0x00);

                        chip.SetRegister(0x06, 0x00);

                        chip.SetRegister(0x07, 0x38);

                        chip.SetRegister(0x08, 0x0f);
                        chip.SetRegister(0x09, 0x0f);
                        chip.SetRegister(0x0a, 0x0f);

                        chip.SetRegister(0x0b, 0x30);
                        chip.SetRegister(0x0c, 0x00);
                        chip.SetRegister(0x0d, 0x04);
                    }

                    const short tp = 284;

                    chip.SetRegister(0x01, (tp >> 8) & 0x000f);
                    chip.SetRegister(0x00, tp & 0x00ff);

                    Console.ReadKey(true);
                }
            }
        }
    }
}
