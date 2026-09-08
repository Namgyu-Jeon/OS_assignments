# 이 스크립트는 공개 저장소에 필요한 문서와 이미지가 존재하고 상대 링크가 올바른지 검사합니다.
# 프로그램을 빌드하지 않고 저장소 문서 구조와 제외 파일만 빠르게 확인합니다.

$ErrorActionPreference = 'Stop'
$repositoryRoot = Split-Path -Parent $PSScriptRoot
$requiredFiles = @(
    'README.md',
    'assignments\assignment-01\README.md',
    'assignments\assignment-01\docs\ASSIGNMENT_REQUIREMENTS.md',
    'assignments\assignment-01\docs\SYSTEM_DESCRIPTION.md',
    'assignments\assignment-01\docs\TEST_DESCRIPTION.md',
    'assignments\assignment-01\docs\USER_GUIDE.md',
    'assignments\assignment-01\docs\SELF_EVALUATION.md',
    'assignments\assignment-01\docs\images\multi-03.png',
    'assignments\assignment-01\docs\images\multi-05.png',
    'assignments\assignment-01\docs\images\multi-08.png',
    'assignments\assignment-01\docs\images\multi-15.png',
    'assignments\assignment-01\project\multi.sln',
    'assignments\assignment-01\project\src\main.cpp',
    'assignments\assignment-01\project\src\app\MultiplicationTableApp.cpp',
    'assignments\assignment-01\project\src\app\MultiplicationTableApp.h',
    'assignments\assignment-01\project\src\input\PositiveIntegerParser.cpp',
    'assignments\assignment-01\project\src\input\PositiveIntegerParser.h',
    'assignments\assignment-01\project\src\output\TableRenderer.cpp',
    'assignments\assignment-01\project\src\output\TableRenderer.h'
)

$failures = [System.Collections.Generic.List[string]]::new()
foreach ($relativePath in $requiredFiles) {
    $absolutePath = Join-Path $repositoryRoot $relativePath
    if (-not (Test-Path -LiteralPath $absolutePath -PathType Leaf)) {
        $failures.Add("필수 파일 없음: $relativePath")
    }
}

$markdownFiles = Get-ChildItem -LiteralPath $repositoryRoot -Filter '*.md' -File -Recurse
foreach ($markdownFile in $markdownFiles) {
    $content = Get-Content -LiteralPath $markdownFile.FullName -Raw -Encoding UTF8
    foreach ($match in [regex]::Matches($content, '!?(?:\[[^\]]*\])\(([^)]+)\)')) {
        $target = $match.Groups[1].Value
        if ($target -match '^(?:https?://|#)') {
            continue
        }

        $pathWithoutAnchor = ($target -split '#', 2)[0]
        if ([string]::IsNullOrWhiteSpace($pathWithoutAnchor)) {
            continue
        }

        $resolvedPath = Join-Path $markdownFile.DirectoryName $pathWithoutAnchor
        if (-not (Test-Path -LiteralPath $resolvedPath)) {
            # Windows PowerShell 5.1에도 GetRelativePath 없이 동작하도록 저장소 루트 접두사를 제거한다.
            $relativeDocument = $markdownFile.FullName.Substring($repositoryRoot.Length).TrimStart('\', '/')
            $failures.Add("끊어진 링크: $relativeDocument -> $target")
        }
    }

    $mermaidCount = ([regex]::Matches($content, '```mermaid')).Count
    if ($mermaidCount -gt 0) {
        $diagramDeclarations = ([regex]::Matches($content, '```mermaid\s*(?:\r?\n)+(?:flowchart|sequenceDiagram|classDiagram|stateDiagram)')).Count
        if ($diagramDeclarations -ne $mermaidCount) {
            $failures.Add("Mermaid 선언 확인 필요: $($markdownFile.FullName)")
        }
    }
}

$trackedFiles = & git -C $repositoryRoot ls-files
foreach ($trackedFile in $trackedFiles) {
    if ($trackedFile -match '(?i)(^|/)(?:\.vs|build|Debug|Release)(?:/|$)' -or
        $trackedFile -match '(?i)\.(?:exe|zip|obj|pdb|ilk|vcxproj\.user)$') {
        $failures.Add("공개 제외 항목이 Git에 포함됨: $trackedFile")
    }
}

if ($failures.Count -gt 0) {
    $failures | ForEach-Object { Write-Error $_ }
    exit 1
}

Write-Output "문서 파일 검사: $($markdownFiles.Count)개 통과"
Write-Output "필수 파일 검사: $($requiredFiles.Count)개 통과"
Write-Output '상대 링크·이미지·Mermaid 선언·제외 파일 검사: 통과'
