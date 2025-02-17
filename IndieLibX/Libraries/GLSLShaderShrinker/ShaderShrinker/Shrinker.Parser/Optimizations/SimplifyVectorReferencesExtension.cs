﻿// -----------------------------------------------------------------------
//  <copyright file="SimplifyVectorReferencesExtension.cs">
//      Copyright (c) 2021 Dean Edis. All rights reserved.
//  </copyright>
//  <summary>
//  This code is provided on an "as is" basis and without warranty of any kind.
//  We do not warrant or make any representations regarding the use or
//  results of use of this code.
//  </summary>
// -----------------------------------------------------------------------

using System;
using System.Linq;
using Shrinker.Lexer;
using Shrinker.Parser.SyntaxNodes;

namespace Shrinker.Parser.Optimizations
{
    public static class SimplifyVectorReferencesExtension
    {
        public static void SimplifyVectorReferences(this SyntaxNode rootNode)
        {
            // Simplify construction from vector .rgba components. (E.g. vec3(v.x, v.y, v.z) => v.xyz)
            foreach (var vectorLength in new[] { 2, 3, 4 })
            {
                var vecType = $"vec{vectorLength}";

                var vectorNodes = rootNode.TheTree
                    .OfType<GenericSyntaxNode>()
                    .Where(o => (o.Token as TypeToken)?.Content == vecType)
                    .Where(o => o.Next is RoundBracketSyntaxNode);
                foreach (var vectorNode in vectorNodes)
                {
                    while (true)
                    {
                        var didChange = false;

                        // Replace 'vec3(v.x, v.y, v.z)' with 'vec3(v.xyz)', etc.
                        var csv = ((RoundBracketSyntaxNode)vectorNode.Next).GetCsv().ToList();
                        var valuesAsVecNames = csv
                            .Select(o => o.Count == 1 ? o.Single() as GenericSyntaxNode : null)
                            .Select(o => o?.Token.Content)
                            .Select(
                                    o =>
                                    {
                                        if (o == null || o.Split('.').Length != 2)
                                            return null;
                                        var indexOfDot = o.IndexOf(".", StringComparison.Ordinal);
                                        if (indexOfDot + 1 >= o.Length || !char.IsLetter(o[indexOfDot + 1]))
                                            return null;
                                        return o;
                                    })
                            .ToList();

                        var newBrackets = new RoundBracketSyntaxNode();
                        for (var i = 0; i < valuesAsVecNames.Count; i++)
                        {
                            var lhsRhs1 = valuesAsVecNames[i]?.Split('.');
                            var lhsRhs2 = i + 1 < valuesAsVecNames.Count ? valuesAsVecNames[i + 1]?.Split('.') : null;

                            if (lhsRhs1 != null && lhsRhs1[1].Any(ch => !"xyzwrgba".Contains(ch)))
                                lhsRhs1 = null;
                            if (lhsRhs2 != null && lhsRhs2[1].Any(ch => !"xyzwrgba".Contains(ch)))
                                lhsRhs2 = null;

                            if (lhsRhs1 == null || lhsRhs2 == null || lhsRhs1[0] != lhsRhs2[0])
                            {
                                // No change.
                                newBrackets.Adopt(csv[i].ToArray());
                                newBrackets.Adopt(new GenericSyntaxNode(new CommaToken()));
                                continue;
                            }

                            // Merge vector components.
                            var newValue = new GenericSyntaxNode($"{lhsRhs1[0]}.{lhsRhs1[1]}{lhsRhs2[1]}");
                            newBrackets.Adopt(newValue, new GenericSyntaxNode(new CommaToken()));

                            i++;
                            didChange = true;
                        }

                        newBrackets.Children.LastOrDefault()?.Remove(); // Remove trailing ','.
                        vectorNode.Next.ReplaceWith(newBrackets);
                        if (!didChange)
                            break; // Stop.
                    }

                    // Remove copy constructors. (E.g. vec2 v = vec2(v_other) => vec2 v = v_other)
                    while (true)
                    {
                        var didChange = false;

                        foreach (var assignment in rootNode
                            .TheTree
                            .OfType<VariableAssignmentSyntaxNode>()
                            .Where(o => o.HasValue &&
                                        o.ValueNodes.First().HasNodeContent(vecType) &&
                                        o.Children.Count == 2 &&
                                        o.Children[1] is RoundBracketSyntaxNode brackets &&
                                        brackets.Children.Count == 1 &&
                                        brackets.Children[0] is GenericSyntaxNode expr &&
                                        expr.Token is AlphaNumToken))
                        {
                            var rhsConstructor = assignment.ValueNodes.First();
                            var brackets = rhsConstructor.Next;
                            var bracketedExpr = (GenericSyntaxNode)brackets.Children.Single();
                            if (bracketedExpr.Token.Content.IndexOfAny(new[] { '.', '[', '(' }) >= 0)
                                continue;

                            // Bracketed expression must be of the correct type.
                            var exprDeclaration = bracketedExpr.FindVarDeclaration();
                            if (exprDeclaration?.VariableType.Content != vecType)
                                continue;

                            // Simplify it...
                            brackets.Remove();
                            rhsConstructor.ReplaceWith(bracketedExpr);
                            didChange = true;
                        }

                        if (!didChange)
                            break; // Stop.
                    }
                }
            }

            // Simplify vectors referencing all their own components. (I.e. v.rgba => v)
            foreach (var node in rootNode.TheTree
                .OfType<GenericSyntaxNode>()
                .Where(o => (o.Token as AlphaNumToken)?.Content.Contains(".") == true)
                .ToList())
            {
                var lhsRhs = node.Token.Content.Split('.');
                if (lhsRhs.Length != 2)
                    continue;

                // Find type of vec.
                var vecType = node.FindVarDeclaration();
                if (vecType == null)
                    continue; // Can't find what type of 'vec' we have.

                // Can we simplify by removing the components?
                var canSimplify = vecType.VariableType.Content switch
                {
                    "vec2" => lhsRhs[1].IsAnyOf("xy", "rg"),
                    "vec3" => lhsRhs[1].IsAnyOf("xyz", "rgb"),
                    "vec4" => lhsRhs[1].IsAnyOf("xyzw", "rgba"),
                    _ => false
                };

                if (canSimplify)
                    node.ReplaceWith(new GenericSyntaxNode(lhsRhs[0]));
            }
        }
    }
}