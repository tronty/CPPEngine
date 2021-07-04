using System;

namespace PerlinVertexShader
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            using (PerlinVertexShader game = new PerlinVertexShader())
            {
                game.Run();
            }
        }
    }
}

