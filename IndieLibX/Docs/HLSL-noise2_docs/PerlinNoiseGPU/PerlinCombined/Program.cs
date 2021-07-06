using System;

namespace PerlinCombined
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            using (PerlinCombined game = new PerlinCombined())
            {
                game.Run();
            }
        }
    }
}

