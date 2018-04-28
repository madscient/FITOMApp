using System;
using System.Threading;
using System.Threading.Tasks;
using SCCISharp;

namespace SampleProgram3
{
    internal class Program
    {
        private static SoundChip _chip;
        private static volatile bool _requiredEnd;

        private static void Main()
        {
            using (var manager = SoundInterfaceManager.GetInstance())
            {
                manager.Reset();
                manager.SetAcquisitionMode(AcquisitionMode.Near);

                if (!manager.TryGetSoundChip(out _chip, ChipType.YM2608, 8.0))
                {
                    Console.Error.WriteLine("このサンプルでは {0} が必要です。", ChipType.YM2608);
                    return;
                }

                using (_chip)
                {
                    manager.ShowLevelDisplay();

                    var task = Task.Run((Action)Play);
                    Console.ReadKey(true);
                    _requiredEnd = true;
                    task.Wait();

                    manager.CloseLevelDisplay();
                }
            }
        }

        private static void Play()
        {
            _chip.SetRegister(0x11, 0x3f);

            _chip.SetRegister(0x18, 0xcf);
            _chip.SetRegister(0x19, 0xcf);
            _chip.SetRegister(0x1a, 0xcf);
            _chip.SetRegister(0x1b, 0xcf);
            _chip.SetRegister(0x1c, 0xcf);
            _chip.SetRegister(0x1d, 0xcf);

            const int bd = 1, sd = 2, top = 4, hh = 8, tom = 16/*, rim = 32*/;

            var score = new[] {
                        top | bd,   0,
                        hh,         0,
                        hh | sd,    0,
                        hh | bd,    hh,
                        hh | bd,    0,
                        hh,         0,
                        hh | sd,    0,
                        hh,         0,

                        hh | bd,    0,
                        hh,         0,
                        hh | sd,    0,
                        hh | bd,    0,
                        hh | bd,    0,
                        hh,         0,
                        hh | sd,    0,
                        hh,         0,

                        hh | bd,    0,
                        hh,         0,
                        hh | sd,    0,
                        hh | bd,    hh,
                        hh | bd,    0,
                        hh,         0,
                        hh | sd,    0,
                        hh,         0,

                        hh | bd,    0,
                        hh,         0,
                        hh | sd,    0,
                        hh | bd,    0,
                        hh | bd,    0,
                        hh,         0,
                        hh | tom,   0,
                        hh | tom,   tom,
                    };

            while (true)
            {
                foreach (var data in score)
                {
                    if (_requiredEnd)
                        return;

                    if (data != 0)
                        _chip.SetRegister(0x10, data);

                    Thread.Sleep(125);
                }
            }
        }
    }
}
