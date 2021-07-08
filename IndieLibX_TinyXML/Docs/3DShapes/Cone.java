
    @Override
    protected final void create() {
        short slices = (short) (4 * Math.cbrt(bottomRadius) + 8);

        mesh = new Mesh(true, slices + 2, slices * 6, VertexAttribute.Position());

        vertices = new float[(slices + 2) * 3];
        float angle = 360f / slices;

        int i = 0;
        for (int s = 0; s < slices; s++) {
            vertices[i++] = bottomRadius * MathUtils.cosDeg(s * angle);
            vertices[i++] = -height / 2;
            vertices[i++] = bottomRadius * MathUtils.sinDeg(s * angle);
        }
        vertices[i++] = vertices[i + 2] = 0;
        vertices[i++] = -height / 2;
        vertices[i + 2] = height / 2;
        vertices[i++] = vertices[i + 2] = 0;

        indices = new short[slices * 6];

        short p = 0;
        for (i = 0; i < indices.length - 6; i += 3) {
            indices[i++] = indices[i + 2] = p++;
            indices[i++] = indices[i + 2] = p;
            indices[i++] = indices[i + 2] = slices;
            indices[i + 2]++;
        }
        indices[i++] = indices[i + 2] = p;
        indices[i++] = indices[i + 2] = 0;
        indices[i++] = indices[i + 2] = slices;
        indices[i + 2]++;

        super.create();
    }

