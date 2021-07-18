using System;

namespace PerlinPixelShader
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            using (PerlinPixelShader game = new PerlinPixelShader())
            {
                game.Run();
            }
        }
    }
}

