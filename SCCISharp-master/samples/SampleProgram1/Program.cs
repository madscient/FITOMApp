using System;
using System.Linq;
using SCCISharp;

namespace SampleProgram1
{
    internal class Program
    {
        private static void Main()
        {
            using (var manager = SoundInterfaceManager.GetInstance())
            {
                manager.Reset();
                DumpInterfaceManager(manager);
            }
        }

        private static void DumpInterfaceManager(SoundInterfaceManager manager)
        {
            Console.WriteLine("SCCI Version: {0}", manager.Version);
            Console.WriteLine("Interface Count: {0}", manager.InterfaceCount);

            var interfaceCount = manager.InterfaceCount;

            for (var i = 0; i < interfaceCount; i++)
            {
                using (var soundInterface = manager.GetInterface(i))
                    DumpInterface(soundInterface, manager.GetInterfaceInfo(i), i);
            }
        }

        private static void DumpInterface(SoundInterface soundInterface, InterfaceInfo info, int number)
        {
            Console.WriteLine("\t#{0} [{1}] ----------", number, info.Name);
            Console.WriteLine("\tDelay: {0} ms", soundInterface.Delay);
            Console.WriteLine("\tLowLevelAPI: {0}support", soundInterface.IsSupportingLowLevelApi ? "" : "not ");
            Console.WriteLine("\tChip Count: {0}", soundInterface.SoundChipCount);

            var chipCount = soundInterface.SoundChipCount;

            for (var i = 0; i < chipCount; i++)
            {
                using (var chip = soundInterface.GetSoundChip(i))
                    DumpChip(chip, i);
            }

            Console.WriteLine();
        }

        private static void DumpChip(SoundChip chip, int number)
        {
            var info = chip.GetInfo();

            Console.WriteLine("\t\t#{0} [{1}] ----------", number, info.Name);
            Console.WriteLine("\t\tType: {0} ({1} Hz)", info.Type, info.Clock);
            Console.WriteLine("\t\tSlot: {0}", info.Slot);
            Console.WriteLine("\t\tLocation: {0}", info.SoundLocation);

            var compatible = info.CompatibleChipType.Zip(info.CompatibleChipClock, (t, c) => $"{t} ({c} Hz)");
            Console.WriteLine("\t\tCompatible: {0}", string.Join(", ", compatible));
            Console.WriteLine("\t\tUsed: {0}", info.IsUsed);

            Console.WriteLine();
        }
    }
}
