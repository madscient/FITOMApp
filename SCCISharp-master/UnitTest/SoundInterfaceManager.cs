using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SCCISharp;

namespace UnitTest
{
    [TestClass]
    public class SoundInterfaceManagerTest
    {
        [TestMethod]
        public void GetInstanceTest()
        {
            var manager1 = SoundInterfaceManager.GetInstance();
            var manager2 = SoundInterfaceManager.GetInstance();

            Assert.IsNotNull(manager1);
            Assert.AreSame(manager1, manager2);

            manager1.Dispose();
            var manager3 = SoundInterfaceManager.GetInstance();

            Assert.IsNotNull(manager3);
            Assert.AreNotSame(manager1, manager3);

            manager3.Dispose();
        }
    }
}
