export type NerdEnumVariant = {
    discriminant: number;
    name: string;
    payloadType?: string;
};

export type NerdEnumDeclaration = {
    name: string;
    variants: NerdEnumVariant[];
};

export function stripNerdLineComment(line: string): string {
    const comment = line.indexOf("--");
    return comment >= 0 ? line.slice(0, comment) : line;
}

export function collectDynamicArrayDeclarationsFromText(text: string, names: Set<string>) {
    const declaration =
        /(?:^|[\s{])([A-Za-z_][A-Za-z0-9_]*)\s*:?\s*\[\s*\.\.\s*\]/g;
    let match: RegExpExecArray | null;
    while ((match = declaration.exec(text)) !== null) {
        names.add(match[1]);
    }
}

export function collectEnumDeclarationsFromText(
    text: string,
    declarations: Map<string, NerdEnumDeclaration>
) {
    const lines = text.split(/\r?\n/);
    for (let i = 0; i < lines.length; ++i) {
        const header = stripNerdLineComment(lines[i]).trim();
        const headerMatch = header.match(/^(?:pub\s+)?([A-Za-z_][A-Za-z0-9_]*)\s*::\s*enum(?:\s*\[[^\]]+\])?\s*\{/);
        if (!headerMatch) {
            continue;
        }

        const name = headerMatch[1];
        const variants: NerdEnumVariant[] = [];
        let nextDiscriminant = 0;
        for (++i; i < lines.length; ++i) {
            const line = stripNerdLineComment(lines[i]).trim();
            if (line.startsWith("}")) {
                break;
            }
            if (!line) {
                continue;
            }

            const variantMatch = line.match(
                /^([A-Za-z_][A-Za-z0-9_]*)(?:\(([^)]*)\))?(?:\s*=\s*(-?(?:0x[0-9a-fA-F]+|\d+)))?/
            );
            if (!variantMatch) {
                continue;
            }

            const explicitDiscriminant = variantMatch[3]
                ? Number.parseInt(
                      variantMatch[3],
                      variantMatch[3].startsWith("0x") ? 16 : 10
                  )
                : undefined;
            const discriminant = Number.isFinite(explicitDiscriminant)
                ? (explicitDiscriminant as number)
                : nextDiscriminant;
            variants.push({
                name: variantMatch[1],
                discriminant,
                payloadType: variantMatch[2]?.trim() || undefined,
            });
            nextDiscriminant = discriminant + 1;
        }

        if (variants.length > 0) {
            declarations.set(name, { name, variants });
        }
    }
}

export function parseUnsignedResult(value: string | undefined): number | undefined {
    if (!value) {
        return undefined;
    }
    const match = value.trim().match(/^(?:0x[0-9a-fA-F]+|\d+)/);
    if (!match) {
        return undefined;
    }
    const parsed = Number.parseInt(match[0], match[0].startsWith("0x") ? 16 : 10);
    return Number.isFinite(parsed) && parsed >= 0 ? parsed : undefined;
}

export function parseIntegerResult(value: string | undefined): number | undefined {
    if (!value) {
        return undefined;
    }
    const text = value.trim();
    const numberMatch = text.match(/^-?(?:0x[0-9a-fA-F]+|\d+)/);
    if (numberMatch) {
        const literal = numberMatch[0];
        const sign = literal.startsWith("-") ? -1 : 1;
        const digits = sign < 0 ? literal.slice(1) : literal;
        const parsed = Number.parseInt(digits, digits.startsWith("0x") ? 16 : 10);
        return Number.isFinite(parsed) ? parsed * sign : undefined;
    }

    const charMatch = text.match(/^'(.*)'$/);
    if (!charMatch) {
        return undefined;
    }
    const body = charMatch[1];
    if (body.startsWith("\\x")) {
        const parsed = Number.parseInt(body.slice(2), 16);
        return Number.isFinite(parsed) ? parsed : undefined;
    }
    if (body.startsWith("\\0")) {
        const parsed = Number.parseInt(body.slice(1), 8);
        return Number.isFinite(parsed) ? parsed : undefined;
    }
    return body.length === 1 ? body.charCodeAt(0) : undefined;
}

export function nerdPrimitiveTypeName(lldbType: string): string {
    const type = lldbType.replace(/\s+/g, " ").trim();
    switch (type) {
        case "_Bool":
        case "bool":
            return "bool";
        case "char":
        case "signed char":
            return "i8";
        case "unsigned char":
            return "u8";
        case "short":
            return "i16";
        case "unsigned short":
            return "u16";
        case "int":
            return "i32";
        case "unsigned int":
            return "u32";
        case "long":
        case "long long":
            return "i64";
        case "unsigned long":
        case "unsigned long long":
            return "u64";
        case "float":
            return "f32";
        case "double":
            return "f64";
        default:
            return type;
    }
}

export function nerdDisplayTypeName(lldbType: string): string {
    const type = lldbType.replace(/\s+/g, " ").trim();
    if (type.endsWith("*")) {
        return `^${nerdDisplayTypeName(type.replace(/\s*\*$/, ""))}`;
    }
    return nerdPrimitiveTypeName(type);
}

export function lldbTypeNameForNerdType(nerdType: string): string {
    const type = nerdType.trim();
    switch (type) {
        case "bool":
            return "bool";
        case "i8":
            return "signed char";
        case "u8":
            return "unsigned char";
        case "i16":
            return "short";
        case "u16":
            return "unsigned short";
        case "i32":
            return "int";
        case "u32":
            return "unsigned int";
        case "i64":
            return "long long";
        case "u64":
            return "unsigned long long";
        case "f32":
            return "float";
        case "f64":
            return "double";
        default:
            return type;
    }
}

export type NerdDynamicArrayDebugType = {
    displayItemType: string;
    itemType: string;
};

export function dynamicArrayDebugTypeFromLldbType(
    lldbType: string | undefined
): NerdDynamicArrayDebugType | undefined {
    if (!lldbType) {
        return undefined;
    }

    const type = lldbType.replace(/\s+/g, " ").trim();
    const namedDynamicArray = type.match(/^\[\s*\.\.\s*\]\s*(.+?)(?:\s*\*)?$/);
    if (namedDynamicArray) {
        const displayItemType = namedDynamicArray[1].trim();
        return {
            displayItemType,
            itemType: lldbTypeNameForNerdType(displayItemType),
        };
    }

    if (!type.endsWith("*")) {
        return undefined;
    }

    const itemType = type.replace(/\s*\*$/, "").trim();
    return {
        displayItemType: nerdPrimitiveTypeName(itemType),
        itemType,
    };
}

export function unsupportedNerdWatchReason(expression: string): string | undefined {
    const text = expression.trim();
    if (!text) {
        return "empty watch expressions are not supported";
    }
    if (/\b(on|for|return|break|continue|defer|assert)\b/.test(text)) {
        return "statement forms are not supported in watches yet";
    }
    if (/:=/.test(text) || /^\s*(let|var)\b/.test(text)) {
        return "declarations are not supported in watches";
    }
    if (/=>/.test(text)) {
        return "pattern branches are not supported in watches yet";
    }
    if (/\.\s*as\s*\(/.test(text)) {
        return "Nerd casts are not supported in watches yet";
    }
    if (/\.\./.test(text)) {
        return "ranges and slices are not supported in watches yet; watch the slice variable or its data/count fields";
    }
    if (/[$]"/.test(text)) {
        return "interpolated strings are not supported in watches";
    }
    if (/[{}]/.test(text)) {
        return "blocks and aggregate literals are not supported in watches yet";
    }
    return undefined;
}

export function enumVariantForTag(
    enumDecl: NerdEnumDeclaration,
    tag: number | undefined
): NerdEnumVariant | undefined {
    if (tag === undefined) {
        return undefined;
    }
    return enumDecl.variants.find((variant) => variant.discriminant === tag);
}

export function enumSummary(enumDecl: NerdEnumDeclaration, tag: number | undefined): string {
    const variant = enumVariantForTag(enumDecl, tag);
    if (!variant) {
        return `${enumDecl.name}.<unknown> (${tag ?? "?"})`;
    }
    return `${enumDecl.name}.${variant.name} (${variant.discriminant})`;
}

export function enumPayloadExpression(baseExpression: string, variant: NerdEnumVariant): string {
    const payloadType = lldbTypeNameForNerdType(variant.payloadType ?? "");
    return `*(${payloadType} *)((char *)&${baseExpression}.payload + ((sizeof(${payloadType}) <= 8) ? 8 : 0))`;
}

export function enumTagFromVariableValue(value: string | undefined): number | undefined {
    if (!value) {
        return undefined;
    }
    const tagMatch = value.match(/\btag\s*[:=]\s*([^,)}]+)/);
    return parseIntegerResult(tagMatch?.[1]);
}
