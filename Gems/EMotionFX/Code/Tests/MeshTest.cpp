/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Tests/TestAssetCode/MeshFactory.h>

#include <AzCore/Math/Vector4.h>
#include <AzCore/std/containers/vector.h>
#include <Tests/SystemComponentFixture.h>
#include <EMotionFX/Source/VertexAttributeLayerAbstractData.h>
#include <EMotionFX/Source/Mesh.h>

namespace EMotionFX
{

    class MeshTextFixture : 
        public SystemComponentFixture
    {
    public:
        void SetUp() override
        {
            SystemComponentFixture::SetUp();
        }

        void TearDown() override
        {
            SystemComponentFixture::TearDown();
        }

    };

    TEST_F(MeshTextFixture, TextConvertColor128To32)
    {
        auto points = AZStd::vector<AZ::Vector3>{
            AZ::Vector3(-1.0f, -1.0f, 0.0f),
            AZ::Vector3(1.0f, -1.0f, 0.0f),
            AZ::Vector3(-1.0f,  1.0f, 0.0f),
        };
        
        AZStd::vector<AZ::Vector4> colors  = AZStd::vector<AZ::Vector4> {
            AZ::Vector4(1.0f, 0.0f, 0.0f, 1.0f),
            AZ::Vector4(0.0f, 1.0f, 0.0f, 1.0f),
            AZ::Vector4(0.0f, 0.0f, 1.0f, 1.0f),
        };

        const auto vertCount = static_cast<uint32>(points.size());
        AZStd::vector<AZ::u32> indices(vertCount);
        std::iota(indices.begin(), indices.end(), 0);
        AZStd::vector<AZ::Vector3> normals {vertCount, {0.0f, 0.0f, 1.0f}};

        EMotionFX::Mesh* mesh = EMotionFX::MeshFactory::Create(indices, points, normals);

        auto* colorLayer =
            EMotionFX::VertexAttributeLayerAbstractData::Create(vertCount, EMotionFX::Mesh::ATTRIB_COLORS128, sizeof(AZ::Vector4));
        mesh->AddVertexAttributeLayer(colorLayer);
        AZStd::copy(colors.begin(), colors.end(), static_cast<AZ::Vector4*>(colorLayer->GetOriginalData()));
        colorLayer->ResetToOriginalData();

        // conver to 32 bit
        mesh->ConvertTo32BitColors();

        auto* attrib = mesh->FindVertexAttributeLayer(EMotionFX::Mesh::ATTRIB_COLORS32);
        ASSERT_NE(attrib, nullptr);
        auto* colors32 = static_cast<AZ::u32*>(attrib->GetOriginalData());
        ASSERT_NE(colors32, nullptr);

        ASSERT_EQ(colors32[0], AZ::Color::CreateFromRgba(255, 0, 0, 255).ToU32());
        ASSERT_EQ(colors32[1], AZ::Color::CreateFromRgba(0, 255, 0, 255).ToU32());
        ASSERT_EQ(colors32[2], AZ::Color::CreateFromRgba(0, 0, 255, 255).ToU32());

        mesh->Destroy();
    }
} // namespace EMotionFX