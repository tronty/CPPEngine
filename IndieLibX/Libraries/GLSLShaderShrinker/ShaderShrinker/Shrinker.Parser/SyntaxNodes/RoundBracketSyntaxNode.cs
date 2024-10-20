﻿// -----------------------------------------------------------------------
//  <copyright file="RoundBracketSyntaxNode.cs">
//      Copyright (c) 2021 Dean Edis. All rights reserved.
//  </copyright>
//  <summary>
//  This code is provided on an "as is" basis and without warranty of any kind.
//  We do not warrant or make any representations regarding the use or
//  results of use of this code.
//  </summary>
// -----------------------------------------------------------------------

using System.Collections.Generic;
using System.Linq;
using Shrinker.Lexer;

namespace Shrinker.Parser.SyntaxNodes
{
    public class RoundBracketSyntaxNode : GroupSyntaxNode
    {
        public override string UiName => "(...)";

        public RoundBracketSyntaxNode()
        {
        }

        public RoundBracketSyntaxNode(IEnumerable<SyntaxNode> nodes)
            : base(nodes)
        {
        }

        public IEnumerable<IList<SyntaxNode>> GetCsv()
        {
            var group = new List<SyntaxNode>();
            foreach (var node in Children)
            {
                if (node.Token is CommaToken)
                {
                    yield return new List<SyntaxNode>(group);
                    group.Clear();
                }
                else
                {
                    group.Add(node);
                }
            }

            if (group.Any())
                yield return group;
        }

        public void RemoveCsvEntry(int index)
        {
            var nodesToRemove = GetCsv().ToList()[index];
            var prevNode = nodesToRemove.FirstOrDefault()?.Previous as GenericSyntaxNode;
            var nextNode = nodesToRemove.LastOrDefault()?.Next as GenericSyntaxNode;

            nodesToRemove.ToList().ForEach(o => o.Remove());

            if (prevNode?.Token is CommaToken && nextNode?.Token is CommaToken)
            {
                prevNode.Remove();
                return;
            }

            if (prevNode == null && nextNode?.Token is CommaToken)
            {
                nextNode.Remove();
                return;
            }

            if (nextNode == null && prevNode?.Token is CommaToken)
                prevNode.Remove();
        }

        protected override SyntaxNode CreateSelf() => new RoundBracketSyntaxNode();

        /// <summary>
        /// Check if the brackets contain a simple sequence of numbers. (E.g. (1, 3, 2))
        /// </summary>
        /// <param name="allowNumericVectors">If true, allow constant vector expressions (E.g. 'vec3(1,2,3)')</param>
        public bool IsNumericCsv(bool allowNumericVectors = false)
        {
            var csv = GetCsv().ToList();
            if (!csv.Any())
                return false;

            for (var i = 0; i < csv.Count; i++)
            {
                if (!IsNumericParam(i, allowNumericVectors))
                    return false;
            }

            return true;
        }

        public bool IsNumericParam(int paramIndex, bool allowNumericVectors = false)
        {
            var csv = GetCsv().ToList();

            var toCheck = csv[paramIndex];
            var subBrackets = toCheck.OfType<RoundBracketSyntaxNode>().ToList();
            if (allowNumericVectors)
            {
                if (subBrackets.Any(o => !o.IsNumericCsv(true)))
                    return false;

                toCheck = toCheck.Where(o => o is not RoundBracketSyntaxNode && o.Token?.Content?.IsAnyOf("vec2", "vec3", "vec4") != true).ToList();
            }
            else
            {
                if (!toCheck.Any())
                    return false;
            }

            return toCheck.All(o => o.Token is INumberToken);
        }
    }
}